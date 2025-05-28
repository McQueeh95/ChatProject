#include "requestitemwidget.h"
#include "ui_requestitemwidget.h"

RequestItemWidget::RequestItemWidget(int requestId, const QString &name, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RequestItemWidget)
{
    ui->setupUi(this);
    this->requestId = requestId;
    ui->requestInfoLabel->setText(name);
    connect(ui->acceptRequesButton, &QAbstractButton::clicked, this, &RequestItemWidget::onAcceptButtonPushed);
    connect(ui->rejectRequestButton, &QAbstractButton::clicked, this, &RequestItemWidget::onRejectButtonPushed);
}

RequestItemWidget::~RequestItemWidget()
{
    delete ui;
}

void RequestItemWidget::onAcceptButtonPushed()
{
    //emit requestAccepted(this->requestId);
    emit requestAction(this->requestId, "accepted");
}

void RequestItemWidget::onRejectButtonPushed()
{
    emit requestAction(this->requestId, "rejected");
}
