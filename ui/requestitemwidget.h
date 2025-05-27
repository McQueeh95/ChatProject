#ifndef REQUESTITEMWIDGET_H
#define REQUESTITEMWIDGET_H

#include <QWidget>

namespace Ui {
class RequestItemWidget;
}

class RequestItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RequestItemWidget(const QString &name, QWidget *parent = nullptr);
    ~RequestItemWidget();

private:
    Ui::RequestItemWidget *ui;
};

#endif // REQUESTITEMWIDGET_H
