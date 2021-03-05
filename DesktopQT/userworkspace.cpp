#include "userworkspace.h"
#include "ui_userworkspace.h"
#include "utils.h"
#include <mainwindow.h>
#include <QListWidgetItem>
#include <QFileDialog>

extern std::string API_ADDR;

UserWorkspace::UserWorkspace(QWidget *parent, std::string user) :
    QMainWindow(parent),
    ui(new Ui::UserWorkspace)
{
    currentUser = user;
    ui->setupUi(this);

    ui->listWidget->setFlow(QListView::LeftToRight);
    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setGridSize(QSize(128,128));
    ui->listWidget->setMovement(QListView::Static);
    ui->listWidget->setViewMode(QListView::IconMode);
    connect(ui->actionLog_out, &QAction::triggered, this, &UserWorkspace::logOut);
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionUpload_4, &QAction::triggered, this, &UserWorkspace::uploadFile);
    fillWorkSpace();
}

UserWorkspace::~UserWorkspace()
{
    delete ui;
}

void UserWorkspace::uploadFile() {
    QFileDialog::getOpenFileNames(this, "Select file to upload.", QDir::homePath());
    fillWorkSpace();
}

void UserWorkspace::logOut() {
    close();
    auto mainWindow = new MainWindow();
    mainWindow->show();
}

void UserWorkspace::fillWorkSpace() {
    ui->listWidget->clear();

    std::string readBuffer;
    std::string addr = API_ADDR + "/files/" + currentUser + "/list";
    makeCurlRequest(addr.c_str(), &readBuffer, NULL);

    json resp;
    resp = json::parse(readBuffer);
    auto files = resp["data"];
    for (auto file : files) {
        auto item = new QListWidgetItem();
        std::string fileName = file["fileName"].get<std::string>();
        std::string fileType = file["fileType"].get<std::string>();
        std::string text = fileName + "." + fileType;
        item->setText(splitStringByLength(fileName).c_str());
        std::string iconLocation = getIcon(fileType);
        item->setIcon(QIcon(iconLocation.c_str()));
        ui->listWidget->addItem(item);
    }
}
