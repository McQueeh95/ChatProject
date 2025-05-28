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
    void onRequestAccepted(int requestId, const QString &action);
public slots:
    void onRequestsListReceived(const QList<std::pair<int, QString>> &requests);
signals:
    void requestAccepted(int requestId, const QString &action);
};

#endif // REQUESTLISTOVERLAY_H
