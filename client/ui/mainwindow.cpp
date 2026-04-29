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
    connect(controller, &AppController::performLogout, this, &MainWindow::showLoginPage);
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

void MainWindow::onLoginSuccess(qint64 userId, const QList<UiStruct::ChatPreview> chats, const QString &username)
{
    m_loginPage->clearInput();
    m_mainPage->setChats(chats);
    m_mainPage->setUserId(userId);
    m_mainPage->setUsername(username);
    qDebug() << "changing window to login";
    this->m_stackedWidget->setCurrentWidget(m_mainPage);
}

void MainWindow::onRegistrationSuccess(qint64 userId, const QString &username)
{
    m_mainPage->setUserId(userId);
    m_mainPage->setUsername(username);
    QString title = "Chat - " + m_controller->getUsername();
    this->setWindowTitle(title);
    this->m_stackedWidget->setCurrentWidget(m_mainPage);
}

