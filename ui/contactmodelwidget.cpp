#include "contactmodelwidget.h"
#include "ui_contactmodelwidget.h"

ContactModelWidget::ContactModelWidget(Contact contact, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ContactModelWidget)
{
    ui->setupUi(this);
    this->contact = contact;
    ui->usernameLabel->setText(contact.username);
}

ContactModelWidget::~ContactModelWidget()
{
    delete ui;
}
