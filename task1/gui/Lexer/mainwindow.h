#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "./ui_mainwindow.h"
#include "src/generateLexer.h"
#include <QFileDialog>
#include <QStandardItemModel>
#include <QTableView>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    bool checkEmptyRegex();

    ~MainWindow();

private
    slots:
            void on_regApplyB_clicked();

    void on_pushB1_clicked();

    void on_pushB2_clicked();

    void on_pushB3_clicked();

    void on_pushB4_clicked();

    void on_openB_clicked();

    void on_saveB_clicked();

    void on_spinBox_valueChanged(int arg1);

    void on_codeGenerate_clicked();

private:
    Ui::MainWindow *ui;
    std::vector <std::string> regexVector;
    int regexIndex;
    Lexer lexer;
    std::vector<std::string> keyMaps;
    QString regexString;
    QString singleReg;
};
#endif // MAINWINDOW_H
