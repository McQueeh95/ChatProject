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
    connect(mainPageWidget, &MainPageWidget::requestAction, this, &MainWindow::onRequestAction);
    connect(mainPageWidget, &MainPageWidget::getMessagesForContact, this, &MainWindow::onGetMessages);
    connect(client, &NetworkClient::addContactRequestSent, this, &MainWindow::onAddContactRequestSent);
    ui->stackedWidget->addWidget(mainPageWidget);
    ui->stackedWidget->setCurrentWidget(mainPageWidget);
    removeRegisterWidget();
    connect(this, &MainWindow::sendContactsToMainWidget, mainPageWidget, &MainPageWidget::onContactsListReceived);
    QList<Contact> contacts = db->getContactList();
    emit sendContactsToMainWidget(contacts);
    connect(this, &MainWindow::sendMessagesToMainWidget, mainPageWidget, &MainPageWidget::onMessagesReceived);
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

void MainWindow::sendMessage(int contactId, const QString &text)
{
    if(!text.isEmpty())
    {
        QString uuid = db->getContactById(contactId);
        client->sendMessage(text, uuid);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    onWindowClosing();
    event->accept();
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
    QList<std::pair<int, QString>> requests = db->getRequests();
    emit sendRequestsToMainWidget(requests);
}

void MainWindow::onRequestAction(int contactId, const QString &action)
{
    if(action == "accepted")
    {
        qDebug() << "on request Action ID FROM MAIN WINDOW: " << contactId;

        db->acceptRequest(contactId);
        QList<Contact> contacts = db->getContactList();
        emit sendContactsToMainWidget(contacts);
        qDebug() << "ID OF CONTACT REQ FROM DB MAINWINDOW"<< db->getRequestById(contactId);
        client->sendContactAccepted(db->getRequestById(contactId));
    }
    else if(action == "rejected")
    {
        db->rejectRequest(contactId);
        client->sendContactRejected(db->getRequestById(contactId));
    }
}

void MainWindow::onWindowClosing()
{
    client->disconnectFromServer();
}

void MainWindow::onGetMessages(int contactId)
{
    emit sendMessagesToMainWidget(db->getMessagesForContact(contactId));
}

void MainWindow::onAddContactRequestSent(const Message &message)
{
    db->addRequestFromCurrent(message.getUuidTo(), message.getTime());
}

void MainWindow::onMessageReceived(const Message &message)
{
    if(message.getType() == 3)
    {
        db->addRequest(message.getUuidFrom(), message.getUsernameFrom(), message.getTime());
    }
    if(message.getType() == 0)
    {
        db->insertMessage(message);
    }
    if(message.getType() == 4)
    {
        qDebug() << "REQUEST CONFIRMED BY: " << message.getUuidFrom();
        db->requestAccepted(message.getUuidFrom(), message.getUsernameFrom());
    }
    if(message.getType() == 5)
    {
        qDebug() << "Request decline by: " << message.getUuidFrom();
        db->requestRejected(message.getUuidFrom(), message.getUsernameFrom());
    }
}

