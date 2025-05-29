#ifndef MAINPAGEWIDGET_H
#define MAINPAGEWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include "requestlistoverlay.h"
#include "../models/contact.h"
//#include "../models/contactlistmodel.h"
#include "../models/contactlistmodel.h"

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
    ContactListModel *contactModel;
    int currentContactId = -1;
    void mousePressEvent(QMouseEvent *event);
private slots:
    void onSendMessageButtonClicked();
    void onAddContactButtonClicked();
    void onUuidReceived(const QString& contactUuid);
    void onShowRequestListButton();
    void onRequestAction(int requestId, const QString &action);
    void onChatSelected(const QModelIndex &current, const QModelIndex &previous);
public slots:
    void onRequestsReceived(const QList<std::pair<int, QString>> &requests);
    void onContactsListReceived(const QList<Contact> &contacts);
signals:
    void messageToSend(int contactId, const QString& text);
    void sendUuidToMainWindow(const QString& contactUuid);
    void requestForAddContactRequests();
    void sendRequestsToList(const QList<std::pair<int, QString>> &requests);
    void requestAction(int requestId, const QString &action);
};

#endif // MAINPAGEWIDGET_H
