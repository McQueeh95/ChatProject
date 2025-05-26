#include "registerwidget.h"
#include "ui_registerwidget.h"

RegisterWidget::RegisterWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegisterWidget)
{
    ui->setupUi(this);
    connect(ui->createAccountButton, &QPushButton::clicked, this, &RegisterWidget::onUsernameButtonClicked);
}

RegisterWidget::~RegisterWidget()
{
    delete ui;
}

void RegisterWidget::onUsernameButtonClicked()
{
    QString username = ui->inputUsernameLineEdit->text();
    emit usernameInputted(username);
}

