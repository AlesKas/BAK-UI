#include "userworkspace.h"
#include "ui_userworkspace.h"
#include "utils.h"
#include <mainwindow.h>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QBoxLayout>
#include <QToolBar>

extern std::string API_ADDR;

UserWorkspace::UserWorkspace(QWidget *parent, std::string user) :
    QMainWindow(parent),
    ui(new Ui::UserWorkspace)
{
    currentUser = user;
    path = "/";
    ui->setupUi(this);

    ui->listWidget->setFlow(QListView::LeftToRight);
    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setGridSize(QSize(128,128));
    ui->listWidget->setMovement(QListView::Static);
    ui->listWidget->setViewMode(QListView::IconMode);
    connect(ui->actionLog_out, &QAction::triggered, this, &UserWorkspace::logOut);
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionUpload_4, &QAction::triggered, this, &UserWorkspace::uploadFile);
    ui->toolBar->setMovable(false);
    fillWorkSpace();
}

UserWorkspace::~UserWorkspace()
{
    delete ui;
}

void UserWorkspace::uploadFile() {
    auto fileName = QFileDialog::getOpenFileName(this, "Select file to upload.", QDir::homePath());
    std::string postFile = fileName.toUtf8().constData();

    std::string addr = API_ADDR + "/files/" + currentUser;
    long returnCode = makePostFileCurlRequest(this, addr.c_str(), postFile.c_str());
    fillWorkSpace();
}

void UserWorkspace::logOut() {
    close();
    auto mainWindow = new MainWindow();
    mainWindow->show();
}

void UserWorkspace::fillWorkSpace() {
    ui->listWidget->clear();
    json j;
    j["directory"] = path;
    std::string readBuffer;

    std::string addr = API_ADDR + "/files/" + currentUser + "/list?directory=" + path;
    makeCurlRequest("GET", addr.c_str(), &readBuffer, j.dump().c_str(), 10);
    json resp;
    resp = json::parse(readBuffer);
    auto files = resp["data"];
    for (auto file : files) {
        auto item = new QListWidgetItem();
        std::string fileName = file["fileName"].get<std::string>();
        std::string fileType = file["fileType"].get<std::string>();
        std::string text = fileName;
        if (fileType != "") {
            text += ".";
            text += fileType;
        }
        item->setText(splitStringByLength(text).c_str());
        std::string iconLocation = getIcon(fileType);
        item->setIcon(QIcon(iconLocation.c_str()));
        ui->listWidget->addItem(item);
    }
}

void UserWorkspace::on_listWidget_itemDoubleClicked(QListWidgetItem *item) {
    std::string itemName = item->text().toUtf8().constData();
    auto index = itemName.rfind(".");
    if (index != std::string::npos) {
        std::cout << "FILE: " << itemName << std::endl;
    } else {
        pathStack.push(path);
        path += itemName + "/";
        fillWorkSpace();
    }
}

void UserWorkspace::on_actioncd_triggered() {
    if (!pathStack.empty()) {
        path = pathStack.top();
        pathStack.pop();
        fillWorkSpace();
    }
}
