#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "createuser.h"
#include "utils.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->input_password->setEchoMode(QLineEdit::Password);
    QPushButton *btnCancel = ui->btn_cancel;
    QObject::connect(btnCancel, SIGNAL(clicked()), this, SLOT(close()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSign_In_triggered()
{
    auto createUserDialog = new CreateUser();
    createUserDialog->show();
}

