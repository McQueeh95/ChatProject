#ifndef REQUESTLISTOVERLAY_H
#define REQUESTLISTOVERLAY_H

#include <QWidget>

namespace Ui {
class RequestListOverlay;
}

class RequestListOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit RequestListOverlay(QWidget *parent = nullptr);
    ~RequestListOverlay();

private:
    Ui::RequestListOverlay *ui;
private slots:

public slots:
    void onRequestsListReceived(const QList<QString> &requests);
};

#endif // REQUESTLISTOVERLAY_H
