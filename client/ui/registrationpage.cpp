#include "registrationpage.h"
#include "ui_registrationpage.h"

#include <QStyle>
#include <QRegularExpression>

RegistrationPage::RegistrationPage(AppController *controller, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegistrationPage)
    , m_controller(controller)
{
    ui->setupUi(this);
    ui->errorLabel->hide();
    connect(controller, &AppController::registrationFailure, this, &RegistrationPage::onRegistrationFailure);
    connect(ui->loginHereButton, &QPushButton::clicked, this, &RegistrationPage::onLoginClicked);
    connect(ui->signUpButton, &QPushButton::clicked, this, &RegistrationPage::onSignUpClicked);
    connect(ui->usernameEdit, &QLineEdit::textChanged, this, &RegistrationPage::clearErrorState);
    connect(ui->passwordEdit, &QLineEdit::textChanged, this, &RegistrationPage::clearErrorState);
    connect(ui->confirmPasswordEdit, &QLineEdit::textChanged, this, &RegistrationPage::clearErrorState);
}

RegistrationPage::~RegistrationPage()
{
    delete ui;
}

void RegistrationPage::onLoginClicked()
{
    ui->usernameEdit->clear();
    ui->passwordEdit->clear();
    ui->confirmPasswordEdit->clear();

    clearErrorState();
    emit loginRequested();
}

void RegistrationPage::onRegistrationFailure()
{
    ui->usernameEdit->setProperty("error", true);

    ui->usernameEdit->style()->unpolish(ui->usernameEdit);
    ui->usernameEdit->style()->polish(ui->usernameEdit);

    ui->errorLabel->setText("User with this name already exists");
    ui->errorLabel->show();
}

void RegistrationPage::onSignUpClicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    QString confirm = ui->confirmPasswordEdit->text();

    QRegularExpression badCharsRx("[^A-Za-z0-9_]");
    if(username.isEmpty() || password.isEmpty() || confirm.isEmpty())
    {
        handleEmptyFields();
        return;
    }
    if(username.length() < 2)
    {
        handleShortUsername();
        return;
    }
    if(username.contains(badCharsRx))
    {
        handleInvalidUsername();
        return;
    }
    if(password.length() < 8)
    {
        handleShortPassword();
        return;
    }
    if(password != confirm)
    {
        handleDifferentPasswords();
        return;
    }
    ui->errorLabel->hide();
    m_controller->createUser(username, password);
}

void RegistrationPage::handleEmptyFields()
{
    if(ui->usernameEdit->text().isEmpty())
    {
        ui->usernameEdit->setProperty("error", true);
        ui->usernameEdit->style()->unpolish(ui->usernameEdit);
        ui->usernameEdit->style()->polish(ui->usernameEdit);
    }

    if(ui->passwordEdit->text().isEmpty())
    {
        ui->passwordEdit->setProperty("error", true);
        ui->passwordEdit->style()->unpolish(ui->passwordEdit);
        ui->passwordEdit->style()->polish(ui->passwordEdit);
    }

    if(ui->confirmPasswordEdit->text().isEmpty())
    {
        ui->confirmPasswordEdit->setProperty("error", true);
        ui->confirmPasswordEdit->style()->unpolish(ui->confirmPasswordEdit);
        ui->confirmPasswordEdit->style()->polish(ui->confirmPasswordEdit);
    }

    ui->errorLabel->setText("Input all the fields!");
    ui->errorLabel->show();
}

void RegistrationPage::handleShortUsername()
{
    ui->usernameEdit->setProperty("error", true);
    ui->usernameEdit->style()->unpolish(ui->usernameEdit);
    ui->usernameEdit->style()->polish(ui->usernameEdit);

    ui->errorLabel->setText("Username must be at leat 2 characters!");
    ui->errorLabel->show();
}

void RegistrationPage::handleInvalidUsername()
{
    ui->usernameEdit->setProperty("error", true);
    ui->usernameEdit->style()->unpolish(ui->usernameEdit);
    ui->usernameEdit->style()->polish(ui->usernameEdit);

    ui->errorLabel->setText("Invalid symbols in the username");
    ui->errorLabel->show();
}

void RegistrationPage::handleShortPassword()
{
    ui->passwordEdit->setProperty("error", true);
    ui->passwordEdit->style()->unpolish(ui->passwordEdit);
    ui->passwordEdit->style()->polish(ui->passwordEdit);

    ui->errorLabel->setText("Password must be at least 8 characters!");
    ui->errorLabel->show();
}

void RegistrationPage::handleDifferentPasswords()
{
    ui->passwordEdit->setProperty("error", true);
    ui->passwordEdit->style()->unpolish(ui->passwordEdit);
    ui->passwordEdit->style()->polish(ui->passwordEdit);

    ui->confirmPasswordEdit->setProperty("error", true);
    ui->confirmPasswordEdit->style()->unpolish(ui->confirmPasswordEdit);
    ui->confirmPasswordEdit->style()->polish(ui->confirmPasswordEdit);

    ui->errorLabel->setText("Passwords are not the same!");
    ui->errorLabel->show();
}

void RegistrationPage::clearErrorState()
{
    ui->usernameEdit->setProperty("error", false);
    ui->passwordEdit->setProperty("error", false);
    ui->confirmPasswordEdit->setProperty("error", false);

    ui->usernameEdit->style()->unpolish(ui->usernameEdit);
    ui->usernameEdit->style()->polish(ui->usernameEdit);

    ui->passwordEdit->style()->unpolish(ui->passwordEdit);
    ui->passwordEdit->style()->polish(ui->passwordEdit);

    ui->confirmPasswordEdit->style()->unpolish(ui->confirmPasswordEdit);
    ui->confirmPasswordEdit->style()->polish(ui->confirmPasswordEdit);

    ui->errorLabel->setText("");
    ui->errorLabel->hide();
}

