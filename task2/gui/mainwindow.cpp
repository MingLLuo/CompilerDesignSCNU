#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QFileDialog"
#include "QStandardItemModel"
#include "src/slr1Parser.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->table->setShowGrid(true);
  ui->table->setGridStyle(Qt::SolidLine);
  ui->table->setSortingEnabled(true);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_openFileB_clicked() {
  QString selectedFilePath =
      QFileDialog::getOpenFileName(this, "Open a file", "", "All Files (*.*)");
  if (selectedFilePath.isEmpty()) {
    return;
  }
  QFile file(selectedFilePath);
  if (!file.open(QIODevice::ReadOnly)) {
    return;
  }
  QTextStream in(&file);
  QString text = in.readAll();
  file.close();
  ui->textEdit->clear();
  ui->textEdit->setPlainText(text);
}

void MainWindow::on_saveFileB_clicked() {
  QString selectedFilePath =
      QFileDialog::getSaveFileName(this, "Save a file", "", "All Files (*.*)");
  if (selectedFilePath.isEmpty()) {
    return;
  }

  QFile file(selectedFilePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return;
  }

  QTextStream out(&file);
  out << ui->textEdit->toPlainText();
  file.close();
}

void MainWindow::on_getFirstFollowB_clicked() {
  QString text = ui->textEdit->toPlainText();
    Grammar grammar(text.toStdString(), "");
  ui->textBrowser->clear();
  ui->textBrowser->setPlainText(QString::fromStdString(
      grammar.firstSetsStr() + '\n' + grammar.followSetsStr()));
}

void displayLR0Table(LR0Parser &parser, QTableView *tableView) {
  // Determine the row and column counts
  int rowCount = parser.itemSets.size();
  int columnCount = parser.symbols.size() + 1; // 1 for state index

  // Create a standard item model with the determined row and column counts
  QStandardItemModel *model =
      new QStandardItemModel(rowCount, columnCount, tableView);

  // Set header data
  model->setHorizontalHeaderItem(0, new QStandardItem(QString("State Index")));
  model->setHorizontalHeaderItem(1, new QStandardItem(QString("Reductions")));

  int column = 2;
  for (auto symbol : parser.symbols) {
    // Separate terminals and non-terminals in headers
    //        QString header = isTerminal(symbol) ? QString("T:") :
    //        QString("NT:");
    QString header;
    header.append(symbol);
    model->setHorizontalHeaderItem(column++, new QStandardItem(header));
  }

  // Iterate over each item set (state)
  int row = 0;
  for (const auto &itemSet : parser.itemSets) {
    // Set state index
    model->setItem(
        row, 0, new QStandardItem("s" + QString::number(itemSet.stateIndex)));
    QString reduceInfo;
    for (auto item : itemSet.items) {
      auto p = item.str();
      if (p.second) { // If it's a reduce item
        reduceInfo += QString::fromStdString("r(" + p.first + "); ");
      }
    }
    model->setItem(row, 1, new QStandardItem(reduceInfo));

    column = 2;
    for (auto symbol : parser.symbols) {
      QString transitionValue = "";

      // Check if there is a transition for the current symbol
      auto transitionIt = parser.transitions[itemSet.stateIndex].find(symbol);
      if (transitionIt != parser.transitions[itemSet.stateIndex].end()) {
        transitionValue = "s" + QString::number(transitionIt->second);
      }

      // Check for reduce actions
      for (auto item : itemSet.items) {
        auto p = item.str();
        if (p.second && item.lhs == parser.productions[0].lhs &&
            item.rhs == parser.productions[0].rhs && symbol == "$")
          transitionValue += "accept";
      }

      // Set the model item for this cell
      model->setItem(row, column++, new QStandardItem(transitionValue));
    }
    row++;
  }
  // Set the model to the table view
  tableView->setModel(model);
}

void MainWindow::on_getLR0_clicked() {
  QString text = ui->textEdit->toPlainText();
  LR0Parser lr0(text.toStdString(), "");
  displayLR0Table(lr0, ui->table);
  ui->textBrowser->clear();
  ui->textBrowser->setPlainText(QString::fromStdString(lr0.itemSetsStr()));
}

void displaySLR1Table(SLR1Parser &parser, QTableView *tableView) {
  // Determine the row and column counts
  int rowCount = parser.itemSets.size();
  int columnCount = parser.symbols.size() + 1; // 1 for state index

  // Create a standard item model with the determined row and column counts
  QStandardItemModel *model =
      new QStandardItemModel(rowCount, columnCount, tableView);

  // Set header data
  model->setHorizontalHeaderItem(0, new QStandardItem(QString("State Index")));

  int column = 1;
  for (auto symbol : parser.symbols) {
    QString header;
    header.append(symbol);
    model->setHorizontalHeaderItem(column++, new QStandardItem(header));
  }

  // Iterate over each item set (state)
  int row = 0;
  for (const auto &itemSet : parser.itemSets) {
    // Set state index
    model->setItem(
        row, 0, new QStandardItem("s" + QString::number(itemSet.stateIndex)));

    column = 1;
    for (auto symbol : parser.symbols) {
      QString transitionValue = "";

      // Check if there is a transition for the current symbol
      auto transitionIt = parser.transitions[itemSet.stateIndex].find(symbol);
      if (transitionIt != parser.transitions[itemSet.stateIndex].end()) {
        transitionValue = "s" + QString::number(transitionIt->second);
      }

      // Check for reduce actions
      for (auto item : itemSet.items) {
        auto p = item.str();
        if (p.second && item.lhs == parser.productions[0].lhs &&
            item.rhs == parser.productions[0].rhs && symbol == "$")
          transitionValue += "accept";
        else if (p.second && parser.followSets[item.lhs].find(symbol) !=
                                 parser.followSets[item.lhs].end()) {
          // If it's a reduce item and the symbol is in the follow set of the
          // item's LHS
          transitionValue += "r(" + QString::fromStdString(p.first) + ")";
        }
      }

      // Set the model item for this cell
      model->setItem(row, column++, new QStandardItem(transitionValue));
    }
    row++;
  }
  // Set the model to the table view
  tableView->setModel(model);
}

void MainWindow::on_getSLR1_clicked() {
  QString text = ui->textEdit->toPlainText();
  SLR1Parser slr1(text.toStdString(), "");
  auto i = slr1.checkValid();
  ui->textBrowser->clear();
  ui->table->clearSpans();
  if (i != 0) {
    if (i == 1) {
      ui->textBrowser->setPlainText(
          QString::fromStdString("reduction and moveIn error"));
    } else {
      ui->textBrowser->setPlainText(
          QString::fromStdString("reduction collison"));
    }
    return;
  }
  displaySLR1Table(slr1, ui->table);
  ui->textBrowser->setPlainText(QString::fromStdString(
      slr1.g.followSetsStr() + '\n' + slr1.itemSetsStr()));
}

void MainWindow::on_parser_clicked()
{
    QString text = ui->textEdit->toPlainText();
    SLR1Parser slr1(text.toStdString(), "");
    auto i = slr1.checkValid();
    ui->textBrowser->clear();
    ui->table->clearSpans();
    if (i != 0) {
        if (i == 1) {
            ui->textBrowser->setPlainText(
                QString::fromStdString("reduction and moveIn error"));
        } else {
            ui->textBrowser->setPlainText(
                QString::fromStdString("reduction collison"));
        }
    }
    displaySLR1Table(slr1, ui->table);
    ui->textBrowser->setPlainText(QString::fromStdString(
        slr1.g.followSetsStr() + '\n' + slr1.itemSetsStr()));

    QString tokens = ui->tokensInput->toPlainText();

    std::vector<std::string> input;
    std::string line;
    for (auto c: tokens.toStdString()) {
        if (c == '\n') {
            if (!line.empty() && line != "Token: comment") {
                input.push_back(line);
            }
            line.clear();
        } else {
            line += c;
        }
    }
    if (!line.empty()) {
        input.push_back(line);
    }

    auto root = slr1.parse(input);
    if (root == nullptr) {
        ui->treeView->setPlainText(QString::fromStdString(("invalid")));
    } else {
        auto tree = slr1.treeNodePrint(root, 0, "");
        ui->treeView->setPlainText(QString::fromStdString((tree)));
    }

}

