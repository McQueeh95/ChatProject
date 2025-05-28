#ifndef CONTACTMODELWIDGET_H
#define CONTACTMODELWIDGET_H

#include <QWidget>
#include "../models/contact.h"

namespace Ui {
class ContactModelWidget;
}

class ContactModelWidget : public QWidget
{
    Q_OBJECT

public:
    ContactModelWidget(Contact contact, QWidget *parent = nullptr);

    ~ContactModelWidget();

private:
    Ui::ContactModelWidget *ui;
    Contact contact;
};

#endif // CONTACTMODELWIDGET_H
