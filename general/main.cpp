#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QDebug>
#include "cpainterface.h"
#include <QPluginLoader>
#include <calibrator.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.show();

    return a.exec();
}
