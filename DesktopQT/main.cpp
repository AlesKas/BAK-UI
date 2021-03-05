#include "mainwindow.h"

#include "utils.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    initApiAddr();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
