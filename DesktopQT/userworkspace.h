#ifndef USERWORKSPACE_H
#define USERWORKSPACE_H

#include <QMainWindow>

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

private:
    std::string currentUser;
    Ui::UserWorkspace *ui;
};

#endif // USERWORKSPACE_H
