#include "mainpage.h"
#include "ui_mainpage.h"
#include "contactwidget.h"
#include "../models/chatdelegate.h"
#include "../models/messagedelegate.h"

MainPage::MainPage(AppController *controller, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainPage)
{
    m_controller = controller;
    ui->setupUi(this);

    m_searchTimer = new QTimer(this);
    m_searchTimer->setSingleShot(true);

    ui->usernameLabel->hide();

    m_chatsModel = new ContactListModel(this);
    ui->chatsList->setModel(m_chatsModel);
    ui->chatsList->setItemDelegate(new ChatDelegate(this));

    m_messages = new MessageViewModel(this);
    ui->messagesList->setModel(m_messages);
    ui->messagesList->setResizeMode(QListView::Adjust);
    ui->messagesList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->messagesList->setWordWrap(true);
    ui->messagesList->setItemDelegate(new MessageDelegate(this));

    m_searchResults = new SearchViewModel(this);

    connect(m_messages, &QAbstractListModel::modelReset, ui->messagesList, &QListView::scrollToBottom);
    connect(m_messages, &QAbstractListModel::rowsInserted, ui->messagesList, &QListView::scrollToBottom);


    connect(ui->chatsList, &QListView::clicked, this, &MainPage::onChatClicked);
    connect(ui->sendMessageButton, &QPushButton::clicked, this, &MainPage::sendMessage);
    connect(m_controller, &AppController::historyReceived, this, &MainPage::showChatHistory);
    connect(m_controller, &AppController::newMessageReceived, this, &MainPage::addNewMessage);
    connect(m_controller, &AppController::localMessageCreated, this, &MainPage::addNewMessage);
    connect(m_controller, &AppController::msgConfirmed, this, &MainPage::ChangeViewStatus);

    connect(ui->searchEdit, &QLineEdit::textEdited, this, &MainPage::onSearchInput);

    connect(m_controller, &AppController::foundUsers, this, &MainPage::showSearchResult);
    //connect(ui->searchEdit, &QLineEdit::inputRejected)
}

MainPage::~MainPage()
{
    delete ui;
}

void MainPage::setChats(const QList<protocol::ChatInfo>& chats)
{

    m_chatsModel->setChats(chats);
}

void MainPage::setUserId(qint64 userId)
{
    m_messages->setUserId(userId);
}

void MainPage::onChatClicked(const QModelIndex &index)
{
    ui->usernameLabel->show();
    qint64 chatId = index.data(Qt::UserRole).toLongLong();
    QString username = index.data(Qt::DisplayRole).toString();
    m_controller->loadHistory(chatId);
    ui->usernameLabel->setText(username);
}

void MainPage::sendMessage()
{
    QString text = ui->messageEdit->toPlainText();
    if(!text.isEmpty())
    {
        m_controller->sendMessage(text);
        ui->messageEdit->clear();
    }
}

void MainPage::showChatHistory(qint64 chatId, const QList<protocol::MsgDeliv> &messages)
{
    if(chatId == m_controller->getCurrentChatId())
    {
        m_messages->setMessages(messages);
    }
}

void MainPage::addNewMessage(const protocol::MsgDeliv &msg)
{
    if(msg.chatId == m_controller->getCurrentChatId())
    {
        m_messages->appendMessage(msg);
    }
}

void MainPage::ChangeViewStatus(const protocol::MsgDeliv &msg)
{
    if(msg.chatId == m_controller->getCurrentChatId())
    {
        m_messages->updateMessage(msg);
    }
}

void MainPage::onSearchInput()
{
    QString toSearch = ui->searchEdit->text();
    if(toSearch.length() > 1)
    {
        ui->chatsList->setModel(m_searchResults);
        m_controller->searchUsers(toSearch);
    }
    else
    {
        ui->chatsList->setModel(m_chatsModel);
    }

}

void MainPage::showSearchResult(const QList<protocol::UserSearch> &users)
{
    m_searchResults->setSearchRes(users);
}
