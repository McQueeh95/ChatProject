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
        userHasUuid(checkUuid.value().first, checkUuid.value().second);
    }
    else
    {
        registerWidget = new RegisterWidget(this);
        ui->stackedWidget->addWidget(registerWidget);
        //When user inputs username complete registration
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
    userHasUuid(uuid, username);
}

void MainWindow::userHasUuid(const QString &uuid, const QString& username)
{
    //Temporary, need to change after
    client = new NetworkClient(QUrl("ws://localhost:8080"));
    client->setUuid(uuid);
    client->setUsername(username);

    connect(client, &NetworkClient::messageReceived, this, &MainWindow::onMessageReceived);

    mainPageWidget = new MainPageWidget(this);
    //Connecting signal from MainWidget for receiving text and give to client
    connect(mainPageWidget, &MainPageWidget::messageToSend, this, &MainWindow::sendMessage);
    connect(mainPageWidget, &MainPageWidget::sendUuidToMainWindow, this, &MainWindow::sendAddContactRequest);
    connect(mainPageWidget, &MainPageWidget::requestForAddContactRequests, this, &MainWindow::onRequestForRequests);
    connect(this, &MainWindow::sendRequestsToMainWidget, mainPageWidget, &MainPageWidget::onRequestsReceived);
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

void MainWindow::sendAddContactRequest(const QString &contactUuid)
{
    if(!contactUuid.isEmpty())
    {
        client->sendAddContactRequest(contactUuid);
    }
}

void MainWindow::onRequestForRequests()
{
    QList<QString> requests = db->getRequests();
    emit sendRequestsToMainWidget(requests);
}

void MainWindow::onMessageReceived(const Message &message)
{
    if(message.getType() == 3)
    {
        db->addRequest(message.getUuidFrom(), message.getUsernameFrom(), message.getTime());
    }
    if(message.getType() == 0)
    {

    }
}

