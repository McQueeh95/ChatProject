#include "registrationpage.h"
#include "ui_registrationpage.h"

RegistrationPage::RegistrationPage(AppController *controller, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegistrationPage)
    , m_controller(controller)
{
    ui->setupUi(this);
    ui->errorLabel->hide();
    connect(ui->loginHereButton, &QPushButton::clicked, this, &RegistrationPage::onLoginClicked);
    connect(ui->signUpButton, &QPushButton::clicked, this, &RegistrationPage::onSignUpClicked);
    //connect(ui->signUpButton, &QPushButton::clicked, this, &RegistrationPage::onUsernameButtonClicked);
}

RegistrationPage::~RegistrationPage()
{
    delete ui;
}

void RegistrationPage::onLoginClicked()
{
    emit loginRequested();
}

void RegistrationPage::onSignUpClicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    QString confirm = ui->confirmPasswordEdit->text();
    if(username.isEmpty() || password.isEmpty() || confirm.isEmpty())
    {
        ui->errorLabel->setText("Fill all fields!");
        ui->errorLabel->show();
        return;
    }
    if(username.length() < 2)
    {
        ui->errorLabel->setText("Username must be at leat 2 characters!");
        ui->errorLabel->show();
        return;
    }
    if(password.length() < 8)
    {
        ui->errorLabel->setText("Password must be at least 8 characters!");
        ui->errorLabel->show();
        return;
    }
    if(password != confirm)
    {
        ui->errorLabel->setText("Passwords are not the same!");
        ui->errorLabel->show();
        return;
    }
    ui->errorLabel->hide();
    m_controller->createUser(username, password);
}

/*void RegistrationPage::onUsernameButtonClicked()
{
    QString username = ui->inputUsernameLineEdit->text();
    emit usernameInputted(username);
}*/

