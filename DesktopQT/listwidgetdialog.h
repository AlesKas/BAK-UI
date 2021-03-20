#ifndef LISTWIDGETDIALOG_H
#define LISTWIDGETDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>

namespace Ui {
class ListWidgetDialog;
}

class ListWidgetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ListWidgetDialog(QWidget *parent = nullptr, std::string arr[] = NULL, int arrSize = 0);
    ~ListWidgetDialog();
    std::vector<std::string> getSelectedItems();

private slots:

private:
    Ui::ListWidgetDialog *ui;
    QWidget *dialogParent;
};

#endif // LISTWIDGETDIALOG_H
