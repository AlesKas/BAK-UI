#include "userworkspace.h"
#include "ui_userworkspace.h"
#include "listwidgetdialog.h"
#include "utils.h"
#include "files.h"
#include <QMimeData>
#include <QDragEnterEvent>
#include <fstream>

extern std::string API_ADDR;
extern std::string APP_DIRECTORY;

UserWorkspace::UserWorkspace(QWidget *parent, std::string user) :
    QMainWindow(parent),
    ui(new Ui::UserWorkspace)
{
    setUpDirs();
    currentUser = user;
    path = "/";
    ui->setupUi(this);
    this->setAcceptDrops(true);
    ui->listWidget->setFlow(QListView::LeftToRight);
    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setGridSize(QSize(128,128));
    ui->listWidget->setMovement(QListView::Static);
    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setAcceptDrops(true);

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
    connect(ui->actionExit, &QAction::triggered, this, &UserWorkspace::cleanAndExit);
    connect(ui->actionUpload_4, &QAction::triggered, this, &UserWorkspace::uploadFile);
    ui->toolBar->setMovable(false);
    pathLabel = new QLabel(this);
    updatePath();
    ui->toolBar->addWidget(pathLabel);
    fillWorkSpace(ui->listWidget, currentUser);
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
        std::string readBuffer;
        fileName.erase(std::remove(fileName.begin(), fileName.end(), '\n'), fileName.end());
        std::string addr = API_ADDR + "/file/" + currentUser + "?fileName=" + path;
        long returnCode = deleteCurlRequest(this, addr, fileName, &readBuffer);
        if (returnCode != 200) {
            json resp = json::parse(readBuffer);
            std::string msg = resp["error"]["error"].get<std::string>();
            showMessaggeBox(msg.c_str(), "Error", QMessageBox::Critical);
        }
        delete item;
        fillWorkSpace(ui->listWidget, currentUser);
    }
}

void UserWorkspace::uploadFile() {
    QFileDialog dialog(this);
    dialog.setDirectory(QDir::homePath());
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setWindowTitle("Select file to upload.");
    //auto fileName = QFileDialog::getOpenFileName(this, "Select file to upload.", QDir::homePath(), QFileDialog::ExistingFiles);
    if (dialog.exec()) {
        QStringList fileNames = dialog.selectedFiles();
        for (auto file : fileNames) {
            std::string fileName = file.toUtf8().constData();
            std::size_t found = fileName.find(".");
            if (found == std::string::npos) {
                showMessaggeBox("Cannot upload files without extension.", "Error", QMessageBox::Critical);
                continue;
            }
            std::string readBuffer;
            std::string addr = API_ADDR + "/files/" + currentUser + "?directory=" + path;
            long returnCode = makePostFileCurlRequest(this, addr.c_str(), fileName.c_str(), &readBuffer);
            if (returnCode != 200) {
                json resp = json::parse(readBuffer);
                std::string msg = resp["error"]["error"].get<std::string>();
                showMessaggeBox(msg.c_str(), "Error", QMessageBox::Critical);
            }
        }
    }
    fillWorkSpace(ui->listWidget, currentUser);
}

void UserWorkspace::logOut() {
    cleanUp();
    close();
    auto mainWindow = new MainWindow();
    mainWindow->show();
}

void UserWorkspace::fillWorkSpace(QListWidget *target, std::string user) {
    target->clear();
    json j;
    j["directory"] = path;
    std::string readBuffer;
    std::string addr;

    if (sharedWorkspace != "") {
        addr = API_ADDR + "/files/" + user + "/list?directory=" + path + "&toUser=" + currentUser;
    } else {
        addr = API_ADDR + "/files/" + user + "/list?directory=" + path + "&toUser=";
    }
    makeCurlRequest("GET", addr.c_str(), &readBuffer, j.dump().c_str(), 10);
    fillWidget(target, readBuffer);
}

void UserWorkspace::on_listWidget_itemDoubleClicked(QListWidgetItem *item) {
    auto name = item->icon().name();
    std::cout << item->icon().name().toUtf8().constData() << std::endl;
    std::string itemName = item->text().toUtf8().constData();
    auto index = itemName.rfind(".");
    if (index != std::string::npos) {
        std::string readBuffer;
        itemName.erase(std::remove(itemName.begin(), itemName.end(), '\n'), itemName.end());
        std::string addr = API_ADDR + "/files/download?user=" + currentUser + "&directory=" + path + "&toUser=&fileName=";
        long returnCode = downloadAndOpen(this, addr, path, itemName, &readBuffer);
        if (returnCode != 200) {
            json resp = json::parse(readBuffer);
            std::string msg = resp["error"]["error"].get<std::string>();
            showMessaggeBox(msg.c_str(), "Error", QMessageBox::Critical);
        }
    } else {
        pathStack.push(path);
        path += itemName + "/";
        fillWorkSpace(ui->listWidget, currentUser);
        updatePath();
    }
}

void UserWorkspace::on_actioncd_triggered() {
    if (ui->tabWidget->currentIndex() == 0) {
        if (!pathStack.empty()) {
            path = pathStack.top();
            pathStack.pop();
            fillWorkSpace(ui->listWidget, currentUser);
            updatePath();
        }
    } else {
        if (!shareStack.empty()) {
            path = shareStack.top();
            shareStack.pop();
            if (path == "/") {
                updatePath();
                shareStack = std::stack<std::string>();
                std::string readBuffer;
                std::string addr = API_ADDR + "/share/" + currentUser + "/list";
                makeCurlRequest("GET", addr.c_str(), &readBuffer, NULL, 10);
                fillWidget(ui->shareWidget, readBuffer);
            } else {
                updatePath();
                fillWorkSpace(ui->shareWidget, sharedWorkspace);
            }
        } else {
            sharedWorkspace = "";
            fillShared();
        }
    }
}

void UserWorkspace::updatePath() {
    pathLabel->setText(path.c_str());
}

void UserWorkspace::on_actionHome_triggered() {
    if (ui->tabWidget->currentIndex() == 0) {
        path = "/";
        pathStack = std::stack<std::string>();
        pathStack.push(path);
        fillWorkSpace(ui->listWidget, currentUser);
        updatePath();
    } else {
        path = "/";
        shareStack = std::stack<std::string>();
        shareStack.push(path);
        fillShared();
        sharedWorkspace = "";
        updatePath();
    }
}

void UserWorkspace::createFolder() {
    bool ok;
    QString folderName = QInputDialog::getText(this, "Dialog", "Enter folder name:", QLineEdit::Normal, "", &ok);

    if(ok && !folderName.isEmpty()) {
        std::string addr = API_ADDR + "/files/folder/" + currentUser + "?directory=" + path + "&folderName=" + folderName.toUtf8().constData();
        std::string returnData;
        makeCurlRequest("PUT", addr.c_str(), &returnData, NULL, 10);
        fillWorkSpace(ui->listWidget, currentUser);
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
        fillWorkSpace(ui->listWidget, currentUser);
    }
}

void UserWorkspace::on_tabWidget_currentChanged(int index) {
    if (index == 0) {
        ui->shareWidget->clear();
        fillWorkSpace(ui->listWidget, currentUser);
        sharedWorkspace = "";
    } else {
        ui->listWidget->clear();
        path = "/";
        pathStack = std::stack<std::string>();
        pathStack.push(path);
        updatePath();
        fillShared();
    }
}

void UserWorkspace::fillShared() {
    ui->shareWidget->clear();
    std::string addr = API_ADDR + "/share/" + currentUser + "/shared";
    std::string readBuffer;
    makeCurlRequest("GET", addr.c_str(), &readBuffer, NULL, 10);

    json resp = json::parse(readBuffer);
    for (auto user : resp["data"]) {
        std::string userName = user.get<std::string>();
        auto item = new QListWidgetItem();
        item->setText(splitStringByLength(userName).c_str());
        std::string iconLocation = ":/icons/img/directory.png";
        item->setIcon(QIcon(iconLocation.c_str()));
        ui->shareWidget->addItem(item);
    }
}

void UserWorkspace::on_shareWidget_itemDoubleClicked(QListWidgetItem *item) {
    if (sharedWorkspace == "") {
        sharedWorkspace = item->text().toUtf8().constData();
        std::string readBuffer;
        std::string addr = API_ADDR + "/share/" + currentUser + "/list";
        makeCurlRequest("GET", addr.c_str(), &readBuffer, NULL, 10);
        fillWidget(ui->shareWidget, readBuffer);
        shareStack.push("/");
    } else {
        std::string itemName = item->text().toUtf8().constData();
        auto index = itemName.rfind(".");
        if (index != std::string::npos) {
            std::string readBuffer;
            itemName.erase(std::remove(itemName.begin(), itemName.end(), '\n'), itemName.end());
            std::string addr = API_ADDR + "/files/download?user=" + sharedWorkspace + "&directory=" + path + "&toUser=" + currentUser + "&fileName=";
            long returnCode = downloadAndOpen(this, addr, path, itemName, &readBuffer);
            std::cout << returnCode;
            if (returnCode != 200) {
                json resp = json::parse(readBuffer);
                std::string msg = resp["error"]["error"].get<std::string>();
                showMessaggeBox(msg.c_str(), "Error", QMessageBox::Critical);
            }
        } else {
            shareStack.push(path);
            path += itemName + "/";
            fillWorkSpace(ui->shareWidget, sharedWorkspace);
            updatePath();
        }
    }

}

void UserWorkspace::fillWidget(QListWidget *target, std::string jsonData) {
    target->clear();
    json resp = json::parse(jsonData);
    for (auto file : resp["data"]) {
        auto item = new QListWidgetItem();
        std::string fileName = file["fileName"].get<std::string>();
        std::string fileType = file["fileType"].get<std::string>();
        bool isDir = file["isDir"].get<bool>();
        std::string text = fileName;
        if (!isDir) {
            std::string iconLocation = getIcon(fileType);
            item->setIcon(QIcon(iconLocation.c_str()));
            if (fileType != "") {
                text += "." + fileType;
            }
            item->setText(splitStringByLength(text).c_str());
            target->addItem(item);
        } else {
            item->setIcon(QIcon(":/icons/img/directory.png"));
            item->setText(splitStringByLength(text).c_str());
            target->addItem(item);
}

    }
}

void UserWorkspace::dropEvent(QDropEvent* event) {
    if (ui->tabWidget->currentIndex() == 0) {
        foreach(QUrl url, event->mimeData()->urls())
        {
            std::string readBuffer;
            std::string filename = url.toLocalFile().toUtf8().constData();
            std::string addr = API_ADDR + "/files/" + currentUser + "?directory=" + path;
            long returnCode = makePostFileCurlRequest(this, addr.c_str(), filename.c_str(), &readBuffer);
            if (returnCode != 200) {
                json resp = json::parse(readBuffer);
                std::string msg = resp["error"]["error"].get<std::string>();
                showMessaggeBox(msg.c_str(), "Error", QMessageBox::Critical);
            }
            fillWorkSpace(ui->listWidget, currentUser);
        }
    } else {
        showMessaggeBox("Cannot drop to share tab.", "Warning", QMessageBox::Warning);
    }
}

void UserWorkspace::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void UserWorkspace::cleanAndExit() {
    cleanUp();
    close();
}
