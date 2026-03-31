#include "ui/mainwindow.h"

#include <iostream>
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AppController controller;
    //Creating window
    MainWindow w(&controller);
    w.show();
    return a.exec();
}
