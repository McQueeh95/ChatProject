#include "ui/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    const QString DB_PATH = "C:/Users/nezna/ChatCourseProjectFull/client/data/client_database.sqlite";
    //Creating window
    MainWindow w(DB_PATH);
    w.show();
    return a.exec();
}
