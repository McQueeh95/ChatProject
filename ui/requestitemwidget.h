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
    explicit RequestItemWidget(int requestId, const QString &name, QWidget *parent = nullptr);
    ~RequestItemWidget();

private:
    Ui::RequestItemWidget *ui;
    int requestId;
private slots:
    void onAcceptButtonPushed();
    void onRejectButtonPushed();
signals:
    void requestAction(int requestId, const QString& action);
};

#endif // REQUESTITEMWIDGET_H
