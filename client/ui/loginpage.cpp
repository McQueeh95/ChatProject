#include "loginpage.h"
#include "ui_loginpage.h"

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
        ui->errorLabel->setText("Input all the fields!");
        ui->errorLabel->show();
        return;
    }
    if(username.length() < 2 || password.length() < 8)
    {
        ui->errorLabel->setText("Wrong username or password!");
        ui->errorLabel->show();
        return;
    }
    m_controller->loginUser(username, password);
}

void LoginPage::onLoginSucces()
{
}

void LoginPage::onLoginFailure()
{
    ui->errorLabel->setText("Wrong username or password!");
    ui->errorLabel->show();
}

void LoginPage::onCreateAccountClicked()
{
    emit registrationRequested();
}
