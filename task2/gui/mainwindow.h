#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_openFileB_clicked();

    void on_saveFileB_clicked();

    void on_getFirstFollowB_clicked();

    void on_getLR0_clicked();

    void on_getSLR1_clicked();

    void on_parser_clicked();

private:
    Ui::MainWindow *ui;
    QString grammerString;
};

static bool isTerminal(char symbol) {
    return !isupper(symbol) && symbol != '@';
}

static bool isEpsilon(char symbol) { return symbol == '@'; }

static bool isNonTerminal(char symbol) { return isupper(symbol); }

#endif // MAINWINDOW_H
