#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "registrationpage.h"
#include "../network/networkclient.h"
#include "../appcontroller.h"
#include "loginpage.h"
#include "mainpage.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(AppController *controller, QWidget *parent = nullptr);
    ~MainWindow();
    void uuidDoesntExist();
    NetworkClient& getClient();
    AppController *m_controller;
    LoginPage *m_loginPage;

private:
    AppController* m_contoler;
    QStackedWidget* m_stackedWidget;
    RegistrationPage* m_registrationPage;
    MainPage* m_mainPage;
    Ui::MainWindow *ui;
    //NetworkClient *client;
    //DataBaseManager *db;
    //Generates uuid and save in DB
    //void handleUserRegistration(const QString& username);
    //Procceed to open main widget
    //void userHasUuid(const QString &uuid, const QString& username);
    //We create registration widget only one time, so better to delete
    //Possible to change logic for deleting other window just add passing value
    //void removeRegistrationPage();
    //void sendMessage(int contactId, const QString& text);
    //void connectSlotForMainPage();
protected:
    //void closeEvent(QCloseEvent *event) override;
private slots:
    void showRegistrationPage();
    void showLoginPage();
    //void onMessageReceived(const Message& message);
    void onLoginSuccess(qint64 userId, const QList<protocol::ChatInfo> chats);
    void onRegistrationSuccess(qint64 userId);

    //REQUEST LIST METHODS
    //GET WHOLE
    //void sendAddContactRequest(const QString& contactUuid);
    //void onRequestForRequests();
    //void onLoginSuccess();


    //void onRequestAction(int contactId, const QString &action);
    //void onWindowClosing();
    //void onGetMessages(int contactId);
    //void onAddContactRequestSent(const Message &message);
    //void onInsertSelfUuidIntoClipboard();
signals:
    //void sendRequestsToMainWidget(const QList<std::pair<int, QString>> &requests);
    //void sendContactsToMainWidget(const QList<Contact> &contacts);
    //void sendMessagesToMainWidget(const QList<DatabaseMessage> &messages);


};
#endif // MAINWINDOW_H
