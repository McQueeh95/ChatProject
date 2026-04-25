#ifndef REGISTRATIONPAGE_H
#define REGISTRATIONPAGE_H

#include <QWidget>
#include "../appcontroller.h"

namespace Ui {
class RegistrationPage;
}

class RegistrationPage : public QWidget
{
    Q_OBJECT

public:
    explicit RegistrationPage(AppController *controller, QWidget *parent = nullptr);
    ~RegistrationPage();

private:
    Ui::RegistrationPage *ui;
    AppController *m_controller;

    void handleErrorInput();
    void handleEmptyFields();
    void handleShortUsername();
    void handleInvalidUsername();
    void handleShortPassword();
    void handleDifferentPasswords();
    void clearErrorState();
signals:
    void loginRequested();
    void registrationRequested(const QString& username, const QString& password);
    void goToMainW(const QString& username);
    void usernameInputted(const QString& username);
private slots:
    void onLoginClicked();
    void onRegistrationFailure();
public slots:
    void onSignUpClicked();
};

#endif // REGISTRATIONPAGE_H
