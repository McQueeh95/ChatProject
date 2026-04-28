#include "mainpage.h"
#include "ui_mainpage.h"
#include "../models/chatdelegate.h"
#include "../models/messagedelegate.h"
#include "../models/approles.h"

#include <QShortcut>
#include <QKeyEvent>

MainPage::MainPage(AppController *controller, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainPage)
{
    m_controller = controller;
    ui->setupUi(this);

    m_searchTimer = new QTimer(this);
    m_searchTimer->setSingleShot(true);

    hideChatScreen();

    setupViews();

    setupConnections();

    renameObjects();
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

void MainPage::setUsername(const QString &username)
{
    m_profilePopUp->setUsername(username);
}

bool MainPage::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->messageEdit && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if(keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
        {
            if(!(keyEvent->modifiers() & Qt::ShiftModifier))
            {
                onMessageSubmitted();
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void MainPage::onChatClicked(const QModelIndex &index)
{
    QString username = index.data(Qt::DisplayRole).toString();
    qint64 chatId = index.data(AppRoles::ChatIdRole).toLongLong();
    qint64 userId = index.data(AppRoles::UserIdRole).toLongLong();
    m_controller->processChatSelection(chatId, userId, username);
}

void MainPage::onMessageSubmitted()
{
    QString text = ui->messageEdit->toPlainText();
    if(!text.isEmpty())
    {
        m_controller->sendMessage(text);
        ui->messageEdit->clear();
    }
}

void MainPage::showChatHistory(qint64 chatId, const QList<UiStruct::Message> &messages)
{
    if(chatId == m_controller->getCurrentChatId())
    {
        m_messages->setMessages(messages);
    }
}

void MainPage::addNewMessage(const UiStruct::Message &msg)
{
    m_messages->appendMessage(msg);
}

void MainPage::changeViewStatus(const UiStruct::Message &msg)
{
    if(msg.chatId == m_controller->getCurrentChatId())
    {
        m_messages->updateMessage(msg);
    }
}

void MainPage::onSearchInput(const QString &text)
{
    if(text.length() >= 2)
    {

        m_searchTimer->start(500);
    }
    else
    {
        m_searchTimer->stop();
        ui->chatsList->setModel(m_chatsModel);
    }

}

void MainPage::onLogout()
{
    m_profilePopUp->hide();
    hideChatScreen();
    ui->searchEdit->clear();
    m_chatsModel->clear();
    m_controller->logout();
}

void MainPage::addNewChat(const protocol::ChatInfo &chat)
{
    m_chatsModel->appendChat(chat);
}

void MainPage::callSearch()
{
    QString toSearch = ui->searchEdit->text();
    m_controller->searchUsers(toSearch);
}

void MainPage::setupConnections()
{

    connect(m_messages, &QAbstractListModel::modelReset, ui->messagesList, &QListView::scrollToBottom);
    connect(m_messages, &QAbstractListModel::rowsInserted, ui->messagesList, &QListView::scrollToBottom);

    QShortcut *escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect (escShortcut, &QShortcut::activated, this, &MainPage::hideChatScreen);
    connect(ui->chatsList, &QListView::clicked, this, &MainPage::onChatClicked);
    connect(ui->searchEdit, &QLineEdit::textChanged, this, &MainPage::onSearchInput);

    connect(ui->sendMessageButton, &QPushButton::clicked, this, &MainPage::onMessageSubmitted);
    connect(ui->backButton, &QPushButton::clicked, this, &MainPage::hideChatScreen);
    connect(ui->profileButton, &QPushButton::clicked, this, &MainPage::showPopUp);

    connect(ui->messageEdit, &QTextEdit::textChanged, this, &MainPage::adjustMessageHeight);

    connect(m_profilePopUp, &ProfilePopUp::logout, this, &MainPage::onLogout);

    connect(m_controller, &AppController::historyReceived, this, &MainPage::showChatHistory);
    connect(m_controller, &AppController::newMessageReceived, this, &MainPage::addNewMessage);
    connect(m_controller, &AppController::chatScreenRequested, this, &MainPage::showChatScreen);
    connect(m_controller, &AppController::localMessageCreated, this, &MainPage::addNewMessage);
    connect(m_controller, &AppController::msgConfirmed, this, &MainPage::changeViewStatus);
    connect(m_controller, &AppController::foundUsers, this, &MainPage::showSearchResult);
    connect(m_controller, &AppController::updateChats, this, &MainPage::addNewChat);
    connect(m_controller, &AppController::noMessagesYet, this, &MainPage::showNoMessagesYet);

    connect(m_searchTimer, &QTimer::timeout, this, &MainPage::callSearch);
}

void MainPage::setupViews()
{
    m_profilePopUp = new ProfilePopUp(this);
    m_profilePopUp->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    QList<qint32> sizes;
    sizes << 280 << 1000;
    ui->splitter->setSizes(sizes);

    m_chatsModel = new ContactListModel(this);
    ui->chatsList->setModel(m_chatsModel);
    ui->chatsList->setItemDelegate(new ChatDelegate(this));
    ui->chatsList->setMouseTracking(true);

    m_messages = new MessageViewModel(this);
    ui->messagesList->setModel(m_messages);
    ui->messagesList->setResizeMode(QListView::Adjust);
    ui->messagesList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->messagesList->setWordWrap(true);
    ui->messagesList->setItemDelegate(new MessageDelegate(this));

    m_searchResults = new SearchViewModel(this);

    ui->messageEdit->installEventFilter(this);
    ui->messageEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    ui->messageEdit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

    ui->chatHintLabel->hide();
    ui->messageEdit->document()->setDocumentMargin(0);
}

void MainPage::renameObjects()
{
    ui->splitter->widget(0)->setObjectName("leftPanel");
    ui->splitter->widget(1)->setObjectName("rightPanel");
}

void MainPage::adjustMessageHeight()
{
    QTextDocument *doc = ui->messageEdit->document();

    doc->setTextWidth(ui->messageEdit->viewport()->width());

    int textHeight = qCeil(doc->size().height());
    int padding = 20;
    int newHeight = textHeight + padding;

    int minHeight = 40;
    int maxHeight = 150;

    if (newHeight < minHeight) newHeight = minHeight;

    if (newHeight <= maxHeight) {
        ui->messageEdit->setFixedHeight(newHeight);
        ui->messageEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    else {
        ui->messageEdit->setFixedHeight(maxHeight);
        ui->messageEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
}

void MainPage::showSearchResult(const QList<protocol::UserSearch> &users)
{
    ui->chatsList->setModel(m_searchResults);
    m_searchResults->setSearchRes(users);
}


void MainPage::hideChatScreen()
{
    ui->peerUsernameLabel->hide();
    ui->sendMessageButton->hide();
    ui->messagesList->hide();
    ui->messageEdit->hide();
    ui->backButton->hide();
    ui->selectChatLabel->show();
    ui->chatsList->clearSelection();
    ui->chatHintLabel->hide();
}

void MainPage::showChatScreen(const QString &username)
{
    ui->peerUsernameLabel->setText(username);
    ui->selectChatLabel->hide();
    ui->sendMessageButton->show();
    ui->messagesList->show();
    ui->messageEdit->show();
    ui->peerUsernameLabel->show();
    ui->backButton->show();
    ui->chatHintLabel->hide();
}

void MainPage::showNoMessagesYet(const QString &username)
{
    ui->peerUsernameLabel->setText(username);
    ui->peerUsernameLabel->show();
    ui->selectChatLabel->hide();
    ui->sendMessageButton->show();
    ui->messagesList->hide();
    ui->messageEdit->show();
    ui->backButton->show();
    ui->chatHintLabel->show();
}

void MainPage::showPopUp()
{
    QPoint globalPos = ui->profileButton->mapToGlobal(QPoint(0, ui->profileButton->height()));

    globalPos.setY(globalPos.y() + 10);
    globalPos.setX(globalPos.x() + 10);
    m_profilePopUp->move(globalPos);
    m_profilePopUp->show();
}
