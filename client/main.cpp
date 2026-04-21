#include "ui/mainwindow.h"

#include <iostream>
#include <QApplication>
#include <QDebug>
#include <sodium.h>

int main(int argc, char *argv[])
{
    if(sodium_init() < 0)
    {
        qDebug() << "Critical Error: Not able to initialize libsodium!";
        return -1;
    }
    QApplication a(argc, argv);
    AppController controller;
    MainWindow w(&controller);
    w.show();
    return a.exec();
}
