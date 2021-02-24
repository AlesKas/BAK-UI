#include "createuser.h"
#include "ui_createuser.h"
#include "qmessagebox.h"
#include "utils.h"
#include <nlohmann/json.hpp>
#include <typeinfo>

using json = nlohmann::json;

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

    QString passwd = ui->PasswdInput->text();
    QString passwdRepeat = ui->PasswdRepeat->text();
    QString userName = ui->UserNameInput->text();
    if (userName == "" || passwd == "" || passwdRepeat == "") {
        QMessageBox msgBox;
        msgBox.setText("Please enter your credentials.");
        msgBox.exec();
        return;
    }
    if (passwd != passwdRepeat) {
         QMessageBox msgBox;
         msgBox.setText("Passwords are not matching");
         msgBox.exec();
         return;
    }

    CURL *curl = curl_easy_init();
    std::string readBuffer;

    if (passwd == passwdRepeat && userName != "") {
        json j;
        j["userName"] = userName.toUtf8().constData();
        j["password"] = passwd.toUtf8().constData();
        std::string jsonData = j.dump();
        std::string addr = API_ADDR + "/users/create";
        long httpCode = makeCurlRequest(addr.c_str(), &readBuffer, curl, jsonData.c_str());

        if (httpCode == 204) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Success");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText("User created successfully.");
            msgBox.exec();
            close();
        } else {
            json j = json::parse(readBuffer);
            auto msg = j["error"]["message"].get<std::string>();
            QMessageBox msgBox;
            msgBox.setWindowTitle("Error");
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText(QString::fromStdString(msg));
            msgBox.exec();
        }
    }
}

void CreateUser::on_buttonBox_rejected()
{
    close();
}

void CreateUser::on_showPasswd_stateChanged(int arg1)
{
    auto checkBox = ui->showPasswd;
    if (checkBox->isChecked()) {
        ui->PasswdInput->setEchoMode(QLineEdit::Normal);
        ui->PasswdRepeat->setEchoMode(QLineEdit::Normal);
    } else {
        ui->PasswdInput->setEchoMode(QLineEdit::Password);
        ui->PasswdRepeat->setEchoMode(QLineEdit::Password);
    }
}
