#include "profilepopup.h"
#include "ui_profilepopup.h"

ProfilePopUp::ProfilePopUp(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::ProfilePopUp)
{
    ui->setupUi(this);
    connect(ui->logoutButton, &QPushButton::clicked, this, &ProfilePopUp::logoutRequsted);
}

ProfilePopUp::~ProfilePopUp()
{
    delete ui;
}

void ProfilePopUp::setUsername(const QString &name)
{
    ui->nameLabel->setText("Logged in as: " + name);
}

void ProfilePopUp::logoutRequsted()
{
    ui->nameLabel->clear();
    emit logout();
}
