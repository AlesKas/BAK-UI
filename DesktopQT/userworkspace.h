#ifndef USERWORKSPACE_H
#define USERWORKSPACE_H

#include <QListWidgetItem>
#include <QMainWindow>
#include <stack>
#include <mainwindow.h>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QFileDialog>
#include <QBoxLayout>
#include <QToolBar>
#include <QLabel>

namespace Ui {
class UserWorkspace;
}

class UserWorkspace : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserWorkspace(QWidget *parent = nullptr, std::string user = "null");
    ~UserWorkspace();
private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_actioncd_triggered();
    void on_actionHome_triggered();
    void deleteItem();
    void createFolder();
    void shareItem();
    void showContextMenu(const QPoint&);
    void uploadFile();
    void on_tabWidget_currentChanged(int index);
    void dropEvent(QDropEvent* event);
    void dragEnterEvent(QDragEnterEvent *e);
    void on_shareWidget_itemDoubleClicked(QListWidgetItem *item);
    void cleanAndExit();

private:
    std::string currentUser;
    Ui::UserWorkspace *ui;
    std::string path;
    std::stack<std::string> pathStack;
    QLabel* pathLabel;
    std::string sharedWorkspace;
    std::stack<std::string> shareStack;

    void logOut();
    void exit();
    void fillWorkSpace(QListWidget *target, std::string user);
    void fillShared();
    void updatePath();
    void fillWidget(QListWidget *target, std::string jsonData);

};

#endif // USERWORKSPACE_H
