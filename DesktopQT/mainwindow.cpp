#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "createuser.h"
#include "userworkspace.h"
#include "utils.h"

extern std::string API_ADDR;

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


void MainWindow::on_btn_login_clicked()
{
    QString userName = ui->input_username->text();
    QString password = ui->input_password->text();
    if (userName == "" || password == "") {
        showMessaggeBox("Please enter your credentials.", "Error", QMessageBox::Critical);
        return;
    }
    std::string readBuffer;

    std::string addr = API_ADDR + "/salt";
    long httpCode = makeCurlRequest(addr.c_str(), &readBuffer, NULL, 5);
    if (httpCode != 200) {
        showMessaggeBox("Error while making API request", "Error", QMessageBox::Critical);
        return;
    }
    json resp = json::parse(readBuffer);
    auto salt = resp["salt"].get<std::string>();
    auto saltedPasswd = password.toUtf8().constData() + salt;
    auto passwdHash = sha256(saltedPasswd);

    addr = API_ADDR + "/users/auth/" + userName.toUtf8().constData() + "/" + passwdHash;
    httpCode = makeCurlRequest(addr.c_str(), &readBuffer, NULL, 5);
    if (httpCode == 200) {
        auto userWorkspace = new UserWorkspace(nullptr, userName.toUtf8().constData());
        userWorkspace->show();
        close();
    } else {
        showMessaggeBox("Incorrect user name or password.", "Error", QMessageBox::Critical);
    }
}

void MainWindow::on_btn_cancel_clicked()
{
    close();
}
