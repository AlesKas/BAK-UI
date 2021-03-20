#include "mainwindow.h"
#include "utils.h"
#include "files.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    initApiAddr();
    setUpDirs();
    MainWindow w;
    w.show();
    a.exec();
    cleanUp();
    return 0;
}
