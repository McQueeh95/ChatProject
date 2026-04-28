#include "loginpage.h"
#include "ui_loginpage.h"

#include <QStyle>

LoginPage::LoginPage(AppController* controller, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginPage)
    , m_controller(controller)
{
    ui->setupUi(this);
    ui->errorLabel->hide();
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
    connect(controller, &AppController::loginSuccess, this, &LoginPage::onLoginSucces);
    connect(controller, &AppController::loginFailure, this, &LoginPage::onLoginFailure);
    connect(ui->usernameEdit, &QLineEdit::textChanged, this, &LoginPage::clearErrorState);
    connect(ui->passwordEdit, &QLineEdit::textChanged, this, &LoginPage::clearErrorState);
    connect(ui->createAccountButton, &QPushButton::clicked, this, &LoginPage::onCreateAccountClicked);
}

LoginPage::~LoginPage()
{
    delete ui;
}

void LoginPage::onLoginClicked()
{
    QString username = this->ui->usernameEdit->text();
    QString password = this->ui->passwordEdit->text();
    if(username.isEmpty() || password.isEmpty())
    {
        handleEmptyFields();
        return;
    }
    if(username.length() < 4 || username.length() > 16 || password.length() < 8)
    {
        handleErrorInput();
        return;
    }
    m_controller->requestSalt(username, password);
}

void LoginPage::onLoginSucces()
{
}

void LoginPage::onLoginFailure()
{
    handleErrorInput();
}

void LoginPage::onCreateAccountClicked()
{
    ui->usernameEdit->clear();
    ui->passwordEdit->clear();

    clearErrorState();
    emit registrationRequested();
}

void LoginPage::clearInput()
{
    ui->usernameEdit->clear();
    ui->passwordEdit->clear();
}

void LoginPage::handleErrorInput()
{
    ui->usernameEdit->setProperty("error", true);
    ui->passwordEdit->setProperty("error", true);

    ui->usernameEdit->style()->unpolish(ui->usernameEdit);
    ui->usernameEdit->style()->polish(ui->usernameEdit);

    ui->passwordEdit->style()->unpolish(ui->passwordEdit);
    ui->passwordEdit->style()->polish(ui->passwordEdit);

    ui->errorLabel->setText("Wrong username or password!");
    ui->errorLabel->show();
}

void LoginPage::handleEmptyFields()
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

    ui->errorLabel->setText("Input all the fields!");
    ui->errorLabel->show();
}

void LoginPage::clearErrorState()
{
    ui->usernameEdit->setProperty("error", false);
    ui->passwordEdit->setProperty("error", false);

    ui->usernameEdit->style()->unpolish(ui->usernameEdit);
    ui->usernameEdit->style()->polish(ui->usernameEdit);

    ui->passwordEdit->style()->unpolish(ui->passwordEdit);
    ui->passwordEdit->style()->polish(ui->passwordEdit);

    ui->errorLabel->setText("");
    ui->errorLabel->hide();
}
