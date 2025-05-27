#include "addcontactpopup.h"
#include "ui_addcontactpopup.h"

AddContactPopup::AddContactPopup(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddContactPopup)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AddContactPopup::onOkButtonClicked);
}

AddContactPopup::~AddContactPopup()
{
    delete ui;
}

void AddContactPopup::onOkButtonClicked()
{
    QString contactUuid = ui->inputUuidLineEdit->text();
    emit uuidToSend(contactUuid);
}
