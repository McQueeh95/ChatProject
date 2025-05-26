#include "mainpagewidget.h"
#include "ui_mainpagewidget.h"

MainPageWidget::MainPageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainPageWidget)
{
    ui->setupUi(this);
    connect(ui->sendMessage, &QPushButton::clicked, this, &MainPageWidget::onSendMessageButtonClicked);
}

MainPageWidget::~MainPageWidget()
{
    delete ui;
}

void MainPageWidget::onSendMessageButtonClicked()
{
    QString text = ui->textEdit->toPlainText();
    emit messageToSend(text);
}
