#include "messageitemwidget.h"
#include "ui_messageitemwidget.h"

MessageItemWidget::MessageItemWidget(const DatabaseMessage &message, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MessageItemWidget)
{
    ui->setupUi(this);
    this->message = message;
    ui->messageText->setText(message.getContent());
}

MessageItemWidget::~MessageItemWidget()
{
    delete ui;
}
