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
    void onRequestActionReceived(int contactId, const QString &action);
    void onInsertSelfUuidClicked();
public slots:
    void onRequestsListReceived(const QList<std::pair<int, QString>> &requests);
signals:
    void requestActionReceived(int contactId, const QString &action);
    void insertSelfUuidIntoClipboard();
};

#endif // REQUESTLISTOVERLAY_H
