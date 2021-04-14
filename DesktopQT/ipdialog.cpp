#include "ipdialog.h"
#include "ui_ipdialog.h"

IPdialog::IPdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IPdialog)
{
    ui->setupUi(this);
    ui->localIP->setInputMask( "000.000.000.000:0000" );
    ui->publicIP->setInputMask( "000.000.000.000:0000" );
}

IPdialog::~IPdialog()
{
    delete ui;
}

void IPdialog::on_checkBox_stateChanged(int arg1) {
    if (ui->checkBox->isChecked()) {
        ui->localIP->setEnabled(false);
        ui->publicIP->setEnabled(false);
    } else {
        ui->localIP->setEnabled(true);
        ui->publicIP->setEnabled(true);
    }
}

void IPdialog::on_buttonBox_accepted() {
    if (ui->checkBox->isChecked()) {
        std::string homeIP = "http://192.168.0.10:8000";
        std::string publicIP = "http://85.160.74.136";
        createConfig(homeIP, publicIP);
        close();
    } else if (!ui->checkBox->isChecked() && ui->localIP->text() != "" && ui->publicIP->text() != "") {
        std::string homeIP = std::string("http://") + ui->localIP->text().toUtf8().constData();
        std::string publicIP = std::string("http://") + ui->publicIP->text().toUtf8().constData();
        createConfig(homeIP, publicIP);
        close();
    } else {
        showMessaggeBox("Please enter storage IP, or use default", "Error", QMessageBox::Critical);
    }
}

void IPdialog::on_buttonBox_rejected() {
    QSettings settings("qt.ini", QSettings::IniFormat);
    settings.beginGroup("API");
    settings.setValue("firstRun", true);
    settings.endGroup();
    exit(-1);
}
