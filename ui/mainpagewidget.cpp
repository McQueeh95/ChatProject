#include "ui_mainpagewidget.h"
#include "addcontactpopup.h"
#include "mainpagewidget.h"
#include "contactmodelwidget.h"

MainPageWidget::MainPageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainPageWidget)
{
    ui->setupUi(this);
    connect(ui->sendMessage, &QPushButton::clicked, this, &MainPageWidget::onSendMessageButtonClicked);
    connect(ui->addContactButton, &QPushButton::clicked, this, &MainPageWidget::onAddContactButtonClicked);
    connect(ui->showRequestListButton, &QPushButton::clicked, this, &MainPageWidget::onShowRequestListButton);
    contactModel = new ContactListModel(this);
    ui->chatList->setModel(contactModel);
    connect(ui->chatList->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &MainPageWidget::onChatSelected);
}

MainPageWidget::~MainPageWidget()
{
    delete ui;
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
        connect(requestListOverlay, &RequestListOverlay::requestAccepted, this, &MainPageWidget::onRequestAction);
        connect(requestListOverlay, &RequestListOverlay::requestAccepted, this, &MainPageWidget::onShowRequestListButton);
        requestListOverlay->raise();
        requestListOverlay->show();

    }
    requestListOverlay->raise();
    requestListOverlay->show();
}

void MainPageWidget::onRequestAction(int requestId, const QString &action)
{
    emit requestAction(requestId, action);
}

void MainPageWidget::onChatSelected(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);

    if(!current.isValid())
        return;

    currentContactId = current.data(Qt::UserRole).toInt();
    qDebug() << "Selected contact id: " << currentContactId;

    /*QList<Message> messages = db->getMessagesForContact(contactId);
    messageModel->setMessages(messages);*/
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

