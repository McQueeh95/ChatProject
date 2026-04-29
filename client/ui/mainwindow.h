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
    AppController *m_controller;
    LoginPage *m_loginPage;

private:
    QStackedWidget* m_stackedWidget;
    RegistrationPage* m_registrationPage;
    MainPage* m_mainPage;
    Ui::MainWindow *ui;

private slots:
    void showRegistrationPage();
    void showLoginPage();
    void onLoginSuccess(qint64 userId, const QList<UiStruct::ChatPreview> chats, const QString &username);
    void onRegistrationSuccess(qint64 userId, const QString &username);

};
#endif // MAINWINDOW_H
