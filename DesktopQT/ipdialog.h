#ifndef IPDIALOG_H
#define IPDIALOG_H

#include <QDialog>
#include "utils.h"

namespace Ui {
class IPdialog;
}

class IPdialog : public QDialog
{
    Q_OBJECT

public:
    explicit IPdialog(QWidget *parent = nullptr);
    ~IPdialog();

private slots:
    void on_checkBox_stateChanged(int arg1);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::IPdialog *ui;
};

#endif // IPDIALOG_H
