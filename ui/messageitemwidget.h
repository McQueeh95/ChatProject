#ifndef MESSAGEITEMWIDGET_H
#define MESSAGEITEMWIDGET_H

#include <QWidget>
#include "../database/databasemessage.h"

namespace Ui {
class MessageItemWidget;
}

class MessageItemWidget : public QWidget
{
    Q_OBJECT

public:
    MessageItemWidget(const DatabaseMessage &message, QWidget *parent = nullptr);
    ~MessageItemWidget();

private:
    Ui::MessageItemWidget *ui;
    DatabaseMessage message;
};

#endif // MESSAGEITEMWIDGET_H
