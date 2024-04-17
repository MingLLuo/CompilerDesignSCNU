#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), regexString("") {
  ui->setupUi(this);
  ui->table->setShowGrid(true);
  ui->table->setGridStyle(Qt::SolidLine);
  ui->table->setSortingEnabled(true);
  this->regexIndex = 0;
  this->regexString = "";
  ui->regText->setPlainText("");
  ui->spinBox->setValue(0);
  ui->spinBox->setMinimum(0);
  ui->spinBox->setMaximum(0);
}

MainWindow::~MainWindow() { delete ui; }

bool MainWindow::checkEmptyRegex() { return this->regexString == ""; }

void MainWindow::on_regApplyB_clicked() {
  this->regexString = ui->regExpInput->toPlainText();
  this->regexVector.clear();
  std::string s = "", singleReg = "";
  if (this->regexString.toStdString().empty()) {
    ui->spinBox->setValue(0);
    ui->spinBox->setMinimum(0);
    ui->spinBox->setMaximum(0);
    return;
  }
  auto str = this->regexString.toStdString();
  // for (auto c : str) {
  //   if (c != '\n') {
  //     s += c;
  //   } else {
  //     this->regexVector.push_back(s);
  //     s.clear();
  //   }
  // }
  // this->regexVector.push_back(s);
  // s.clear();

  lexer.setPattern(str, "");

  this->singleReg = QString::fromStdString(singleReg);
  this->keyMaps.clear();
  for (auto &[k,v]: this->lexer.dfas) {
      this->keyMaps.push_back(k);
  }
  ui->spinBox->setMinimum(1);
  ui->spinBox->setMaximum(this->lexer.dfas.size());
  ui->spinBox->setValue(1);
}

void displayNFAInTableView(std::shared_ptr<NFA> nfa, QTableView *tableView) {
  int rowCount = nfa->states.size();
  int columnCount =
      nfa->symbols.size() + 4; // 1 for state id, 1 for start, 1 for final state, 1 for final status

  // Create a standard item model
  QStandardItemModel *model =
      new QStandardItemModel(rowCount, columnCount, tableView);

  // Set header data
  model->setHorizontalHeaderItem(0, new QStandardItem(QString("State ID")));
  model->setHorizontalHeaderItem(1, new QStandardItem(QString("Start")));
  model->setHorizontalHeaderItem(2, new QStandardItem(QString("Final")));
  model->setHorizontalHeaderItem(3, new QStandardItem(QString("Final Status")));

  int column = 4;
  for (char symbol : nfa->symbols) {
    if (symbol == 0) {
      model->setHorizontalHeaderItem(column++, new QStandardItem(QString("ε")));
      continue;
    }
    model->setHorizontalHeaderItem(column++,
                                   new QStandardItem(QString(symbol)));
  }

  int row = 0;
  for (const auto &state : nfa->states) {
    // Set state ID
    model->setItem(row, 0, new QStandardItem(QString::number(state->id)));

    // Set start state
    model->setItem(row, 1,
                   new QStandardItem(state == nfa->start_state ? "Yes" : ""));
    // Set final state
    model->setItem(row, 2, new QStandardItem(state->is_final ? "Yes" : ""));

    // Set final status
    model->setItem(row, 3,
                   new QStandardItem(QString(state->final_status.c_str())));

    // Set transition symbols
    column = 4;
    for (char symbol : nfa->symbols) {
      std::string transitions;
      for (const auto &nextState : state->transitions[symbol]) {
        transitions += std::to_string(nextState->id) + ",";
      }
      transitions = transitions.empty()
                        ? "-"
                        : transitions.substr(0, transitions.size() - 1);
      model->setItem(row, column++,
                     new QStandardItem(QString(transitions.c_str())));
    }
    row++;
  }

  // Set the model to the table view
  tableView->setModel(model);
}

void displayDFAInTableView(std::shared_ptr<DFA> dfa, QTableView *tableView) {
  int rowCount = dfa->dfa_states.size();
  int columnCount =
      dfa->symbols.size() + 4; // 1 for state id, 1 for start, 1 for final state, 1 for final status

  // Create a standard item model
  QStandardItemModel *model =
      new QStandardItemModel(rowCount, columnCount, tableView);

  // Set header data
  model->setHorizontalHeaderItem(0, new QStandardItem(QString("State ID")));
  model->setHorizontalHeaderItem(1, new QStandardItem(QString("Start")));
  model->setHorizontalHeaderItem(2, new QStandardItem(QString("Final")));
  model->setHorizontalHeaderItem(3, new QStandardItem(QString("Final Status")));

  int column = 4;
  for (char symbol : dfa->symbols) {
    model->setHorizontalHeaderItem(column++,
                                   new QStandardItem(QString(symbol)));
  }

  int row = 0;
  for (const auto &state : dfa->dfa_states) {
    if (state == nullptr)
      continue;
    // Set state ID
    model->setItem(row, 0, new QStandardItem(QString::number(state->id)));

    // Set start state
    model->setItem(row, 1,
                   new QStandardItem(state == dfa->start_state ? "Yes" : ""));
    // Set final state
    model->setItem(row, 2, new QStandardItem(state->is_final ? "Yes" : ""));
    // Set final status
    model->setItem(row, 3,
                   new QStandardItem(QString(state->final_status.c_str())));

    // Set transition symbols
    column = 4;
    for (char symbol : dfa->symbols) {
      if (symbol == 0) {
        model->setHorizontalHeaderItem(column++,
                                       new QStandardItem(QString("ε")));
        continue;
      }

      auto transition = state->transitions.find(symbol);
      std::string transitions;
      if (transition != state->transitions.end() &&
          transition->second != nullptr) {
        transitions += std::to_string(transition->second->id);
      } else {
        transitions = "-";
      }
      model->setItem(row, column++,
                     new QStandardItem(QString(transitions.c_str())));
    }
    row++;
  }

  // Set the model to the table view
  tableView->setModel(model);
}

void MainWindow::on_pushB1_clicked() {
  if (this->checkEmptyRegex()) {
    return;
  }

  if (ui->checkALL->isChecked()) {
    std::shared_ptr<NFA> nfa = std::make_shared<NFA>();
    displayNFAInTableView(nfa, ui->table);
  } else {
    auto token = this->keyMaps[this->regexIndex];
    auto nfa = lexer.regExps[token]->toNFA();
    displayNFAInTableView(nfa, ui->table);
  }
}

void MainWindow::on_pushB2_clicked() {
  if (this->checkEmptyRegex()) {
    return;
  }
  if (ui->checkALL->isChecked()) {
    auto &dfa = lexer.finalDFA;
    displayDFAInTableView(dfa, ui->table);
  } else {
    auto token = this->keyMaps[this->regexIndex];
    auto nfa = lexer.regExps[token]->toNFA();
    auto dfa = convertToDFA(nfa);
    displayDFAInTableView(dfa, ui->table);
  }
}

void MainWindow::on_pushB3_clicked() {
  if (this->checkEmptyRegex()) {
    return;
  }
  if (ui->checkALL->isChecked()) {
    auto &dfa = lexer.finalDFA;
    displayDFAInTableView(dfa, ui->table);
  } else {
    auto token = this->keyMaps[this->regexIndex];
    auto nfa = lexer.regExps[token]->toNFA();
    auto dfa = convertToDFA(nfa)->minimizeDFA();
    displayDFAInTableView(dfa, ui->table);
  }
}

void MainWindow::on_pushB4_clicked() {
  if (this->checkEmptyRegex()) {
    return;
  }
  auto fileCode = generateLexer(lexer);
  generateLexerToFile(lexer, "hello.cpp");
  ui->textBrowser->setText(QString::fromStdString(fileCode));
}

void MainWindow::on_openB_clicked() {
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
  ui->regExpInput->clear();
  ui->regExpInput->setPlainText(text);
}

void MainWindow::on_saveB_clicked() {
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
  out << ui->regExpInput->toPlainText();
  file.close();
}

void MainWindow::on_spinBox_valueChanged(int arg1) {
  this->regexIndex = arg1 - 1;
  if (ui->checkALL->isChecked()) {
    ui->regText->setPlainText(this->singleReg);
  } else {
    ui->regText->setPlainText(
        QString::fromStdString(this->keyMaps[this->regexIndex]));
  }
}

void MainWindow::on_codeGenerate_clicked() {
  QString selectedFilePath =
      QFileDialog::getSaveFileName(this, "Save a file", "", "All Files (*.*)");
  if (selectedFilePath.isEmpty()) {
    return;
  }

  QFile file(selectedFilePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return;
  }
  if (this->checkEmptyRegex()) {
    return;
  }
  auto fileCode = generateLexer(lexer);
  QTextStream out(&file);
  out << QString::fromStdString(fileCode);
  file.close();
}
