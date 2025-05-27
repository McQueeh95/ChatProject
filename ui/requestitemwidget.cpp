#include "requestitemwidget.h"
#include "ui_requestitemwidget.h"

RequestItemWidget::RequestItemWidget(const QString &name, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RequestItemWidget)
{
    ui->setupUi(this);
    ui->requestInfoLabel->setText(name);
}

RequestItemWidget::~RequestItemWidget()
{
    delete ui;
}
