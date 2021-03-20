#include "listwidgetdialog.h"
#include "ui_listwidgetdialog.h"

ListWidgetDialog::ListWidgetDialog(QWidget *parent, std::string arr[], int arrSize) :
    QDialog(parent),
    ui(new Ui::ListWidgetDialog)
{
    ui->setupUi(this);
    dialogParent = parent;
    ui->listWidget->setSelectionMode(QListWidget::MultiSelection);
    for (int i = 0; i < arrSize; i++) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(arr[i].c_str());
        ui->listWidget->addItem(item);
    }
}

ListWidgetDialog::~ListWidgetDialog()
{
    delete ui;
}

std::vector<std::string> ListWidgetDialog::getSelectedItems() {
    std::vector<std::string> ret;
    for (auto item : ui->listWidget->selectedItems()) {
        ret.push_back(item->text().toUtf8().constData());
    }
    return ret;
}
