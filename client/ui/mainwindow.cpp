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

    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);

    m_loginPage = new LoginPage(m_controller, this);
    this->m_stackedWidget->addWidget(m_loginPage);


    m_registrationPage = new RegistrationPage(m_controller, this);
    this->m_stackedWidget->addWidget(m_registrationPage);

    m_mainPage = new MainPage(m_controller, this);
    this->m_stackedWidget->addWidget(m_mainPage);

    this->m_stackedWidget->setCurrentWidget(m_loginPage);

    connect(m_loginPage, &LoginPage::registrationRequested, this, &MainWindow::showRegistrationPage);
    connect(m_registrationPage, &RegistrationPage::loginRequested, this, &MainWindow::showLoginPage);

    connect(controller, &AppController::loginSuccess, this, &MainWindow::onLoginSuccess);
    connect(controller, &AppController::registrationSuccess, this, &MainWindow::onRegistrationSuccess);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showRegistrationPage()
{
    this->m_stackedWidget->setCurrentWidget(m_registrationPage);
}

void MainWindow::showLoginPage()
{
    this->m_stackedWidget->setCurrentWidget(m_loginPage);
}

void MainWindow::onLoginSuccess(qint64 userId, const QList<protocol::ChatInfo> chats)
{
    m_mainPage->setChats(chats);
    m_mainPage->setUserId(userId);
    QString title = "Chat - " + m_controller->getUsername();
    this->setWindowTitle(title);
    this->m_stackedWidget->setCurrentWidget(m_mainPage);
}

void MainWindow::onRegistrationSuccess(qint64 userId)
{
    m_mainPage->setUserId(userId);
    QString title = "Chat - " + m_controller->getUsername();
    this->setWindowTitle(title);
    this->m_stackedWidget->setCurrentWidget(m_mainPage);
}

