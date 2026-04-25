#include "ui/mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QStyleFactory>
#include <QDebug>
#include <QFontDatabase>
#include <sodium.h>


int main(int argc, char *argv[])
{
    if(sodium_init() < 0)
    {
        qDebug() << "Critical Error: Not able to initialize libsodium!";
        return -1;
    }

    QApplication a(argc, argv);

    QFontDatabase::addApplicationFont(":/fonts/fonts/Inter_18pt-Black.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Inter_18pt-Bold.ttf");

    a.setStyle(QStyleFactory::create("Fusion"));
    QFile styleFile(":/theme/styles.qss");

    if(styleFile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream stream(&styleFile);
        QString stylesheet = stream.readAll();

        a.setStyleSheet(stylesheet);

        styleFile.close();
        qDebug() << "Styles successfully downloaded";
    }



    AppController controller;
    MainWindow w(&controller);
    w.show();
    return a.exec();
}
