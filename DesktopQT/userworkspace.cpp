#include "userworkspace.h"
#include "ui_userworkspace.h"
#include "utils.h"

UserWorkspace::UserWorkspace(QWidget *parent, std::string user) :
    QMainWindow(parent),
    ui(new Ui::UserWorkspace)
{
    currentUser = user;
    ui->setupUi(this);
}

UserWorkspace::~UserWorkspace()
{
    delete ui;
}
