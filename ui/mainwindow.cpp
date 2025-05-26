#include <QUuid>
#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(const QString& dbPath, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    db = new DataBaseManager(dbPath);
    //Checking if id is present in DB
    auto checkUuid = db->getUuid();
    if(checkUuid.has_value())
    {
        userHasUuid(checkUuid.value());
        //Connecting signal from MainWidget for receiving text and give to client
        connect(mainPageWidget, &MainPageWidget::messageToSend, this, &MainWindow::sendMessage);
    }
    else
    {
        registerWidget = new RegisterWidget(this);
        ui->stackedWidget->addWidget(registerWidget);
        //When user inputs nickname complete registration
        connect(registerWidget, &RegisterWidget::usernameInputted, this, &MainWindow::handleUserRegistration);
        uuidDoesntExist();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::uuidDoesntExist()
{
    ui->stackedWidget->setCurrentWidget(registerWidget);
}

NetworkClient &MainWindow::getClient()
{
    return *client;
}

void MainWindow::handleUserRegistration(const QString &username)
{
    QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    db->registerUser(uuid, username);
    userHasUuid(uuid);
}

void MainWindow::userHasUuid(const QString &uuid)
{
    //Temporary, need to change after
    client = new NetworkClient(QUrl("ws://localhost:8080"));
    client->setUuid(uuid);

    mainPageWidget = new MainPageWidget(this);
    ui->stackedWidget->addWidget(mainPageWidget);
    ui->stackedWidget->setCurrentWidget(mainPageWidget);

    removeRegisterWidget();
}

void MainWindow::removeRegisterWidget()
{
    if(registerWidget != nullptr)
    {
        ui->stackedWidget->removeWidget(registerWidget);
        registerWidget->deleteLater();
        registerWidget = nullptr;
    }
}

void MainWindow::sendMessage(const QString &text)
{
    if(!text.isEmpty())
    {
        //Just for testing purposes
        client->sendMessage(text, "abe40c5a-a764-4499-8d6a-955fd43d8e54");
    }
}

