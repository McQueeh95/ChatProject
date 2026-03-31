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
    this->m_stackedWidget->setCurrentWidget(m_mainPage);
}

void MainWindow::onRegistrationSuccess(qint64 userId)
{
    m_mainPage->setUserId(userId);
    this->m_stackedWidget->setCurrentWidget(m_mainPage);
}

/*void MainWindow::uuidDoesntExist()
{
    ui->stackedWidget->setCurrentWidget(registerWidget);
}*/

/*NetworkClient &MainWindow::getClient()
{
    return *client;
}*/

/*void MainWindow::handleUserRegistration(const QString &username)
{
    while(!username.isEmpty())
    {
    QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    db->registerUser(uuid, username);
    userHasUuid(uuid, username);
    break;
    }
}*/

/*void MainWindow::userHasUuid(const QString &uuid, const QString& username)
{
    //Temporary, need to change after
    client = new NetworkClient(QUrl("ws://localhost:8080"));
    client->setUuid(uuid);
    client->setUsername(username);

    connect(client, &NetworkClient::messageReceived, this, &MainWindow::onMessageReceived);

    MessagesListModel *messagesModel = new MessagesListModel(this);
    messagesModel->setUuid(client->getUuid());
    ContactListModel *contactModel = new ContactListModel(this);
    MessageItemDelegate *messageDelegate = new MessageItemDelegate(client->getUuid());
    ContactIemDelegate *contactDelegate = new ContactIemDelegate();

    mainPageWidget = new MainPageWidget(contactModel, messagesModel, this);
    mainPageWidget->getMessageListView()->setItemDelegate(messageDelegate);
    mainPageWidget->getContactsListView()->setItemDelegate(contactDelegate);
    connectSlotForMainPage();
    //Connecting signal from MainWidget for receiving text and give to client

    ui->stackedWidget->addWidget(mainPageWidget);
    ui->stackedWidget->setCurrentWidget(mainPageWidget);
    removeRegisterWidget();

    connect(this, &MainWindow::sendContactsToMainWidget, mainPageWidget, &MainPageWidget::onContactsListReceived);
    QList<Contact> contacts = db->getContactList();
    emit sendContactsToMainWidget(contacts);
    connect(this, &MainWindow::sendMessagesToMainWidget, mainPageWidget, &MainPageWidget::onMessagesReceived);
}*/

/*void MainWindow::removeRegisterWidget()
{
    if(registerWidget != nullptr)
    {
        ui->stackedWidget->removeWidget(registerWidget);
        registerWidget->deleteLater();
        registerWidget = nullptr;
    }
}*/

/*void MainWindow::sendMessage(int contactId, const QString &text)
{
    if(!text.isEmpty())
    {
        QString uuid = db->getContactById(contactId);
        Message msg = client->sendMessage(text, uuid);
        db->insertOwnMessage(msg);
        onGetMessages(this->mainPageWidget->getCurrentContactId());
    }
}

void MainWindow::connectSlotForMainPage()
{
    connect(mainPageWidget, &MainPageWidget::messageToSend, this, &MainWindow::sendMessage);
    connect(mainPageWidget, &MainPageWidget::sendUuidToMainWindow, this, &MainWindow::sendAddContactRequest);
    connect(mainPageWidget, &MainPageWidget::requestForAddContactRequests, this, &MainWindow::onRequestForRequests);
    connect(this, &MainWindow::sendRequestsToMainWidget, mainPageWidget, &MainPageWidget::onRequestsReceived);
    connect(mainPageWidget, &MainPageWidget::requestAction, this, &MainWindow::onRequestAction);
    connect(mainPageWidget, &MainPageWidget::getMessagesForContact, this, &MainWindow::onGetMessages);
    connect(client, &NetworkClient::addContactRequestSent, this, &MainWindow::onAddContactRequestSent);
    connect(mainPageWidget, &MainPageWidget::insertSelfUuidIntoClipboard, this, &MainWindow::onInsertSelfUuidIntoClipboard);
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

void MainWindow::onLoginSuccess()
{
    qDebug() << "Login Success";
}

void MainWindow::onRequestAction(int contactId, const QString &action)
{
    if(action == "accepted")
    {

        db->acceptRequest(contactId);
        QList<Contact> contacts = db->getContactList();
        emit sendContactsToMainWidget(contacts);
        client->sendContactAccepted(db->getRequestById(contactId));
    }
    else if(action == "rejected")
    {
        db->rejectRequest(contactId);
        client->sendContactRejected(db->getRequestById(contactId));
    }
    onRequestForRequests();
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

void MainWindow::onInsertSelfUuidIntoClipboard()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(client->getUuid());
    ui->statusbar->showMessage("Uuid copied into clipboard");
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
        onGetMessages(this->mainPageWidget->getCurrentContactId());
    }
    if(message.getType() == 4)
    {
        db->requestAccepted(message.getUuidFrom(), message.getUsernameFrom());
        QList<Contact> contacts = db->getContactList();
        emit sendContactsToMainWidget(contacts);
    }
    if(message.getType() == 5)
    {
        db->requestRejected(message.getUuidFrom(), message.getUsernameFrom());
    }
}*/

