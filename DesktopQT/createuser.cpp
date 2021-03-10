#include "createuser.h"
#include "ui_createuser.h"
#include "utils.h"

extern std::string API_ADDR;

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
        showMessaggeBox("Please enter your credentials.", "Error", QMessageBox::Critical);
        return;
    }
    if (passwd != passwdRepeat) {
        showMessaggeBox("Passwords are not matching.", "Error", QMessageBox::Critical);
        return;
    }

    if (passwd == passwdRepeat && userName != "") {
        json j;
        std::string readBuffer;

        std::string addr = API_ADDR + "/salt";
        long httpCode = makeCurlRequest(addr.c_str(), &readBuffer, NULL, 5);
        if (httpCode != 200) {
            showMessaggeBox("Error while making API request", "Error", QMessageBox::Critical);
            return;
        }
        json resp = json::parse(readBuffer);
        auto salt = resp["salt"].get<std::string>();
        auto saltedPasswd = passwd.toUtf8().constData() + salt;
        auto passwdHash = sha256(saltedPasswd);

        j["userName"] = userName.toUtf8().constData();
        j["password"] = passwdHash;
        addr = API_ADDR + "/users/create";
        readBuffer = "";
        httpCode = makeCurlRequest(addr.c_str(), &readBuffer, j.dump().c_str(), 10);

        if (httpCode == 204) {
            showMessaggeBox("User created successfully.", "Success", QMessageBox::Information);
            close();
        } else {
            json j = json::parse(readBuffer);
            auto msg = j["error"]["message"].get<std::string>();
            showMessaggeBox(msg.c_str(), "Error", QMessageBox::Critical);
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
