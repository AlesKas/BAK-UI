#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "createuser.h"

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
    void on_actionSign_In_triggered();

    void on_btn_login_clicked();

    void on_btn_cancel_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
