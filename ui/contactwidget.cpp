#include "contactwidget.h"
#include "ui_contactwidget.h"

contactWidget::contactWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::contactWidget)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_TransparentForMouseEvents);
}

contactWidget::~contactWidget()
{
    delete ui;
}

void contactWidget::setContactInfo(const QString& username)
{
    ui->usernameLabel->setText(username);
    ui->avatarLabel->setText(username.left(1).toUpper());

    ui->avatarLabel->setStyleSheet(
        "background-color: #8774e1; "
        "color: white; "
        "border-radius: 20px; "
        "font-size: 16px; "
        "font-weight: bold;"
        );
}
