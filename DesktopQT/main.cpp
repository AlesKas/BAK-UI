#include "mainwindow.h"
#include "utils.h"
#include "files.h"
#include "ipdialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (isFirstRun()) {
        IPdialog dialog;
        dialog.exec();
    }
    initApiAddr();
    MainWindow w;
    w.show();
    a.exec();
    cleanUp();
    return 0;
}
