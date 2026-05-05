#include <QUuid>
#include <QClipboard>
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "../models/contactiemdelegate.h"


MainWindow::MainWindow(AppController *controller, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_controller(controller)
{
    ui->setupUi(this);

    this->resize(1200, 800);

    ui->networkErrorLabel->hide();

    m_loginPage = new LoginPage(m_controller, this);
    ui->stackedWidget->addWidget(m_loginPage);


    m_registrationPage = new RegistrationPage(m_controller, this);
    ui->stackedWidget->addWidget(m_registrationPage);

    m_mainPage = new MainPage(m_controller, this);
    ui->stackedWidget->addWidget(m_mainPage);

    ui->stackedWidget->setCurrentWidget(m_loginPage);

    connect(m_loginPage, &LoginPage::registrationRequested, this, &MainWindow::showRegistrationPage);
    connect(m_registrationPage, &RegistrationPage::loginRequested, this, &MainWindow::showLoginPage);

    connect(controller, &AppController::loginSuccess, this, &MainWindow::onLoginSuccess);
    connect(controller, &AppController::registrationSuccess, this, &MainWindow::onRegistrationSuccess);
    connect(controller, &AppController::performLogout, this, &MainWindow::showLoginPage);
    connect(controller, &AppController::networkStateChanged, this, &MainWindow::onNetworkStateChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showRegistrationPage()
{
    ui->stackedWidget->setCurrentWidget(m_registrationPage);
}

void MainWindow::showLoginPage()
{
    ui->stackedWidget->setCurrentWidget(m_loginPage);
}

void MainWindow::onLoginSuccess(qint64 userId, const QList<UiStruct::ChatPreview> chats, const QString &username)
{
    m_loginPage->clearInput();
    m_mainPage->setChats(chats);
    m_mainPage->setUserId(userId);
    m_mainPage->setUsername(username);
    ui->stackedWidget->setCurrentWidget(m_mainPage);
}

void MainWindow::onRegistrationSuccess(qint64 userId, const QString &username)
{
    m_mainPage->setUserId(userId);
    m_mainPage->setUsername(username);
    ui->stackedWidget->setCurrentWidget(m_mainPage);
}

void MainWindow::onNetworkStateChanged(bool state)
{
    if(state == false)
    {
        ui->networkErrorLabel->show();
    }
    else
    {
        ui->networkErrorLabel->hide();
    }
}

