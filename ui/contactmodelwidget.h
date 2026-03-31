#ifndef CONTACTMODELWIDGET_H
#define CONTACTMODELWIDGET_H

#include <QWidget>
#include "../networktypes.h"

namespace Ui {
class ContactModelWidget;
}

class ContactModelWidget : public QWidget
{
    Q_OBJECT

public:
    ContactModelWidget(QWidget *parent = nullptr);
    void setChat(const protocol::ChatInfo chat);
    ~ContactModelWidget();

private:
    Ui::ContactModelWidget *ui;
    protocol::ChatInfo chat;
};

#endif // CONTACTMODELWIDGET_H
