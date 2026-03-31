#include "contactmodelwidget.h"
#include "ui_contactmodelwidget.h"

ContactModelWidget::ContactModelWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ContactModelWidget)
{
    ui->setupUi(this);
}

ContactModelWidget::~ContactModelWidget()
{
    delete ui;
}

void ContactModelWidget::setChat(const protocol::ChatInfo chat)
{
    this->chat = chat;
    ui->usernameLabel->setText(chat.peerUsername);
}
