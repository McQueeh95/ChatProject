#ifndef MAINPAGEWIDGET_H
#define MAINPAGEWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include "requestlistoverlay.h"

namespace Ui {
class MainPageWidget;
}

class MainPageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainPageWidget(QWidget *parent = nullptr);
    ~MainPageWidget();

private:
    Ui::MainPageWidget *ui;
    RequestListOverlay *requestListOverlay;
    void mousePressEvent(QMouseEvent *event);
private slots:
    void onSendMessageButtonClicked();
    void onAddContactButtonClicked();
    void onUuidReceived(const QString& contactUuid);
    void onShowRequestListButton();
public slots:
    void onRequestsReceived(const QList<QString> requests);
signals:
    void messageToSend(const QString& text);
    void sendUuidToMainWindow(const QString& contactUuid);
    void requestForAddContactRequests();
    void sendRequestsToList(const QList<QString>& requests);
};

#endif // MAINPAGEWIDGET_H
