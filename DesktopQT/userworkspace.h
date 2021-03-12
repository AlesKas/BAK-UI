#ifndef USERWORKSPACE_H
#define USERWORKSPACE_H

#include <QListWidgetItem>
#include <QMainWindow>
#include <stack>

namespace Ui {
class UserWorkspace;
}

class UserWorkspace : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserWorkspace(QWidget *parent = nullptr, std::string user = "null");
    ~UserWorkspace();
    void logOut();
    void exit();
    void uploadFile();
    void fillWorkSpace();

private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_actioncd_triggered();

private:
    std::string currentUser;
    Ui::UserWorkspace *ui;
    std::string path;
    std::stack<std::string> pathStack;
};

#endif // USERWORKSPACE_H
