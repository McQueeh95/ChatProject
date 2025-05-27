#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "registerwidget.h"
#include "mainpagewidget.h"
#include "../network/networkclient.h"
#include "../network/message.h"
#include "../database/databasemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString& dbPath, QWidget *parent = nullptr);
    ~MainWindow();
    void uuidDoesntExist();
    NetworkClient& getClient();

private:
    QStackedWidget* stacketWidget;
    RegisterWidget* registerWidget;
    MainPageWidget* mainPageWidget;
    Ui::MainWindow *ui;
    NetworkClient *client;
    DataBaseManager *db;
    //Generates uuid and save in DB
    void handleUserRegistration(const QString& username);
    //Procceed to open main widget
    void userHasUuid(const QString &uuid, const QString& username);
    //We create registration widget only one time, so better to delete
    //Possible to change logic for deleting other window just add passing value
    void removeRegisterWidget();
    void sendMessage(const QString& text);
private slots:
    void onMessageReceived(const Message& message);
    void sendAddContactRequest(const QString& contactUuid);
    void onRequestForRequests();
signals:
    QList<QString> sendRequestsToMainWidget(const QList<QString> &requests);

};
#endif // MAINWINDOW_H
