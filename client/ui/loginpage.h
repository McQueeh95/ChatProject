#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>
#include "../appcontroller.h"

namespace Ui {
class LoginPage;
}

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(AppController* controller, QWidget *parent = nullptr);
    void clearInput();
    ~LoginPage();

private slots:
    void onLoginClicked();
    void onLoginSucces();
    void onLoginFailure();
    void onCreateAccountClicked();
signals:
    void registrationRequested();
    void successfullLogin();

private:
    Ui::LoginPage *ui;
    AppController* m_controller;
    void handleErrorInput();
    void handleEmptyFields();
    void clearErrorState();

};

#endif // LOGINPAGE_H
