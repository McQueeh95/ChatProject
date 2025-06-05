#include "ui_mainpagewidget.h"
#include "addcontactpopup.h"
#include "mainpagewidget.h"

MainPageWidget::MainPageWidget(ContactListModel *contactModel, MessagesListModel *messageModel, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainPageWidget)
{
    ui->setupUi(this);
    connect(ui->sendMessage, &QPushButton::clicked, this, &MainPageWidget::onSendMessageButtonClicked);
    connect(ui->addContactButton, &QPushButton::clicked, this, &MainPageWidget::onAddContactButtonClicked);
    connect(ui->showRequestListButton, &QPushButton::clicked, this, &MainPageWidget::onShowRequestListButton);

    this->contactModel = contactModel;
    ui->chatList->setModel(contactModel);
    ui->chatList->clearSelection();
    ui->chatList->setCurrentIndex(QModelIndex());

    messagesModel = messageModel;
    ui->messageArea->setModel(messagesModel);

    connect(ui->chatList->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &MainPageWidget::onChatSelected);
    ui->chatList->clearSelection();
    ui->chatList->setCurrentIndex(QModelIndex());
    ui->messageArea->setUniformItemSizes(false);
    ui->messageArea->setWordWrap(true);
    setMessageSendingUi();
}

MainPageWidget::~MainPageWidget()
{
    delete ui;
}

int MainPageWidget::getCurrentContactId() const
{
    return currentContactId;
}

QListView* MainPageWidget::getMessageListView()
{
    return ui->messageArea;
}

QListView *MainPageWidget::getContactsListView()
{
    return ui->chatList;
}

void MainPageWidget::setMessageSendingUi()
{
    ui->splitter->setStretchFactor(0, 0);
    ui->splitter->setStretchFactor(1, 1);
    if(currentContactId == -1)
    {
        ui->messageArea->setVisible(false);
        ui->nickname->setVisible(false);
        ui->textEdit->setVisible(false);
        ui->sendMessage->setVisible(false);
    }
}

void MainPageWidget::mousePressEvent(QMouseEvent *event)
{

    if(requestListOverlay && requestListOverlay->isVisible())
    {
        if (!requestListOverlay->geometry().contains(event->pos()))
        {
            requestListOverlay->close();
            requestListOverlay->deleteLater();
            requestListOverlay = nullptr;
        }
    }
    QWidget::mousePressEvent(event);
}

void MainPageWidget::onSendMessageButtonClicked()
{
    QString text = ui->textEdit->toPlainText();
    ui->textEdit->clear();
    emit messageToSend(currentContactId, text);
}

void MainPageWidget::onAddContactButtonClicked()
{
    AddContactPopup *addContactPopup = new AddContactPopup(this);
    connect(addContactPopup, &AddContactPopup::uuidToSend, this, &MainPageWidget::onUuidReceived);
    addContactPopup->show();
}

void MainPageWidget::onUuidReceived(const QString& contactUuid)
{
    emit sendUuidToMainWindow(contactUuid);
}

void MainPageWidget::onShowRequestListButton()
{
    if(!requestListOverlay)
    {
        requestListOverlay = new RequestListOverlay(this);
        emit requestForAddContactRequests();
        //connect(requestListOverlay, &RequestListOverlay::uuid)
        connect(requestListOverlay, &RequestListOverlay::requestActionReceived, this, &MainPageWidget::onRequestAction);
        connect(requestListOverlay, &RequestListOverlay::requestActionReceived, this, &MainPageWidget::onShowRequestListButton);
        connect(requestListOverlay, &RequestListOverlay::insertSelfUuidIntoClipboard, this, &MainPageWidget::onInsertUuidIntoClipboard);
        requestListOverlay->raise();
        requestListOverlay->show();
    }
    requestListOverlay->raise();
    requestListOverlay->show();
}

void MainPageWidget::onRequestAction(int contactId, const QString &action)
{
    //qDebug() << "on request action id: " << contactId;
    emit requestAction(contactId, action);
}

void MainPageWidget::onChatSelected(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    qDebug() << currentContactId;
    if(!current.isValid())
        return;
    ui->messageArea->setVisible(true);
    ui->nickname->setVisible(true);
    ui->textEdit->setVisible(true);
    ui->sendMessage->setVisible(true);
    currentContactId = current.data(Qt::UserRole).toInt();
    ui->nickname->setText(current.data(Qt::DisplayRole).toString());
    emit getMessagesForContact(currentContactId);
}

void MainPageWidget::onRequestsReceived(const QList<std::pair<int, QString>> &requests)
{
    connect(this, &MainPageWidget::sendRequestsToList, requestListOverlay, &RequestListOverlay::onRequestsListReceived);
    emit sendRequestsToList(requests);
}

void MainPageWidget::onContactsListReceived(const QList<Contact> &contacts)
{
    contactModel->setContacts(contacts);
}

void MainPageWidget::onMessagesReceived(const QList<DatabaseMessage> &messages)
{
    messagesModel->setMessages(messages);
}

void MainPageWidget::onInsertUuidIntoClipboard()
{
    qDebug() << "MainPageWidget";
    emit insertSelfUuidIntoClipboard();
}

