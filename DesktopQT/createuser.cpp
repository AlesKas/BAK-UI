#include "createuser.h"
#include "ui_createuser.h"
#include "qmessagebox.h"
#include <curl/curl.h>
#include <iostream>

CreateUser::CreateUser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateUser)
{
    ui->setupUi(this);
    ui->PasswdInput->setEchoMode(QLineEdit::Password);
    ui->PasswdRepeat->setEchoMode(QLineEdit::Password);
}

CreateUser::~CreateUser()
{
    delete ui;
}

void CreateUser::on_buttonBox_accepted()
{
    CURL *curl = curl_easy_init();
    CURLcode res;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://stackoverflow.com/");
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        std::cout << res << std::endl;
    }
   /* QString passwd = ui->PasswdInput->text();
    QString passwdRepeat = ui->PasswdRepeat->text();
    if (passwd != passwdRepeat) {
        QMessageBox msgBox;
        msgBox.setText("Passwords are not matching");
        msgBox.exec();
    }*/
}

void CreateUser::on_buttonBox_rejected()
{
    close();
}
