#include "ui_mainpagewidget.h"
#include "addcontactpopup.h"
#include "mainpagewidget.h"

MainPageWidget::MainPageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainPageWidget)
{
    ui->setupUi(this);
    connect(ui->sendMessage, &QPushButton::clicked, this, &MainPageWidget::onSendMessageButtonClicked);
    connect(ui->addContactButton, &QPushButton::clicked, this, &MainPageWidget::onAddContactButtonClicked);
    connect(ui->showRequestListButton, &QPushButton::clicked, this, &MainPageWidget::onShowRequestListButton);
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
    emit messageToSend(text);
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
        requestListOverlay->raise();
        requestListOverlay->show();

    }
    requestListOverlay->raise();
    requestListOverlay->show();
}

void MainPageWidget::onRequestsReceived(const QList<QString> requests)
{
    connect(this, &MainPageWidget::sendRequestsToList, requestListOverlay, &RequestListOverlay::onRequestsListReceived);
    emit sendRequestsToList(requests);
}
