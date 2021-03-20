#include "userworkspace.h"
#include "ui_userworkspace.h"
#include "listwidgetdialog.h"
#include "utils.h"
#include "files.h"

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

    ui->shareWidget->setFlow(QListView::LeftToRight);
    ui->shareWidget->setResizeMode(QListView::Adjust);
    ui->shareWidget->setGridSize(QSize(128,128));
    ui->shareWidget->setMovement(QListView::Static);
    ui->shareWidget->setViewMode(QListView::IconMode);

    ui->tabWidget->setTabText(0, "My files");
    ui->tabWidget->setTabText(1, "Shared files");
    ui->tabWidget->setCurrentIndex(0);

    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(const QPoint&)));

    connect(ui->actionLog_out, &QAction::triggered, this, &UserWorkspace::logOut);
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionUpload_4, &QAction::triggered, this, &UserWorkspace::uploadFile);
    ui->toolBar->setMovable(false);
    pathLabel = new QLabel(this);
    updatePath();
    ui->toolBar->addWidget(pathLabel);
    fillWorkSpace();
}

UserWorkspace::~UserWorkspace() {
    delete ui;
}

void UserWorkspace::showContextMenu(const QPoint& pos) {
    QPoint globalPos = ui->listWidget->mapToGlobal(pos);

    QMenu menu;
    if (ui->listWidget->selectedItems().size() != 0) {
        menu.addAction("Delete", this, SLOT(deleteItem()));
        menu.addAction("Share with...", this, SLOT(shareItem()));
    }
    menu.addAction("Create folder", this, SLOT(createFolder()));
    menu.addAction("Upload file", this, SLOT(uploadFile()));
    menu.exec(globalPos);
}

void UserWorkspace::deleteItem() {
    for (int i = 0; i < ui->listWidget->selectedItems().size(); i++) {
        QListWidgetItem *item = ui->listWidget->takeItem(ui->listWidget->currentRow());
        std::string fileName = item->text().toUtf8().constData();
        fileName.erase(std::remove(fileName.begin(), fileName.end(), '\n'), fileName.end());
        std::string addr = API_ADDR + "/file/" + currentUser + "?fileName=" + path;
        long returnCode = deleteCurlRequest(this, addr, fileName);
        delete item;
        fillWorkSpace();
    }
}

void UserWorkspace::uploadFile() {
    auto fileName = QFileDialog::getOpenFileName(this, "Select file to upload.", QDir::homePath());
    std::string postFile = fileName.toUtf8().constData();
    if (postFile != "") {
        std::string addr = API_ADDR + "/files/" + currentUser + "?directory=" + path;
        long returnCode = makePostFileCurlRequest(this, addr.c_str(), postFile.c_str());
        fillWorkSpace();
    }
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
    json resp = json::parse(readBuffer);
    for (auto file : resp["data"]) {
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
        itemName.erase(std::remove(itemName.begin(), itemName.end(), '\n'), itemName.end());
        std::string addr = API_ADDR + "/files/download?user=" + currentUser + "&directory=" + path + "&fileName=";
        downloadAndOpen(this, addr, path, itemName);
    } else {
        pathStack.push(path);
        path += itemName + "/";
        fillWorkSpace();
        updatePath();
    }
}

void UserWorkspace::on_actioncd_triggered() {
    if (!pathStack.empty()) {
        path = pathStack.top();
        pathStack.pop();
        fillWorkSpace();
        updatePath();
    }
}

void UserWorkspace::updatePath() {
    pathLabel->setText(path.c_str());
}

void UserWorkspace::on_actionHome_triggered() {
    path = "/";
    pathStack = std::stack<std::string>();
    pathStack.push(path);
    fillWorkSpace();
    updatePath();
}

void UserWorkspace::createFolder() {
    bool ok;
    QString folderName = QInputDialog::getText(this, "Dialog", "Enter folder name:", QLineEdit::Normal, "", &ok);

    if(ok && !folderName.isEmpty()) {
        std::string addr = API_ADDR + "/files/folder/" + currentUser + "?directory=" + path + "&folderName=" + folderName.toUtf8().constData();
        std::string returnData;
        makeCurlRequest("PUT", addr.c_str(), &returnData, NULL, 10);
        fillWorkSpace();
    } else if (ok && folderName.isEmpty()) {
        showMessaggeBox("Please enter name.", "Error", QMessageBox::Warning);
        createFolder();
    }
}

void UserWorkspace::shareItem() {
    std::string addr = API_ADDR + "/users/" + currentUser;
    std::string readBuffer;
    makeCurlRequest("GET", addr.c_str(), &readBuffer, NULL, 10);
    json resp = json::parse(readBuffer);
    auto users = resp["data"];
    std::string arr[users.size()];
    for (int i = 0; i < users.size(); i++) {
        arr[i] = users[i].get<std::string>();
    }

    ListWidgetDialog *dialog = new ListWidgetDialog(this, arr, users.size());
    dialog->setModal(true);
    if (dialog->exec() == QDialog::Accepted) {
        auto selectedUser = dialog->getSelectedItems();
        for (auto item : selectedUser) {
            for (int i = 0; i < ui->listWidget->selectedItems().size(); i++) {
                QListWidgetItem *selectedItem = ui->listWidget->takeItem(ui->listWidget->currentRow());
                std::string fileName = selectedItem->text().toUtf8().constData();
                fileName.erase(std::remove(fileName.begin(), fileName.end(), '\n'), fileName.end());
                addr = API_ADDR + "/share/" + currentUser + "?toUser=" + item + "&directory=" + path + "&fileName=" + fileName;
                makeCurlRequest("PUT", addr.c_str(), &readBuffer, NULL, 10);
            }
        }
        fillWorkSpace();
    }
}

void UserWorkspace::on_tabWidget_currentChanged(int index) {
    if (index == 0) {
        fillWorkSpace();
    } else {
        fillShared();
    }
}

void UserWorkspace::fillShared() {

}
