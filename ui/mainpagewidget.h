#ifndef MAINPAGEWIDGET_H
#define MAINPAGEWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include "requestlistoverlay.h"
#include "../models/contact.h"
//#include "../models/contactlistmodel.h"
#include "../models/contactlistmodel.h"
#include "../network/message.h"
#include "../models/messageslistmodel.h"
#include "../database/databasemessage.h"

namespace Ui {
class MainPageWidget;
}

class MainPageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainPageWidget(QWidget *parent = nullptr);
    ~MainPageWidget();
    int getCurrentContactId() const;

private:
    Ui::MainPageWidget *ui;
    RequestListOverlay *requestListOverlay;
    ContactListModel *contactModel;
    MessagesListModel *messagesModel;
    int currentContactId = -1;
    void mousePressEvent(QMouseEvent *event);
private slots:
    void onSendMessageButtonClicked();
    void onAddContactButtonClicked();
    void onUuidReceived(const QString& contactUuid);
    void onShowRequestListButton();
    void onRequestAction(int contactId, const QString &action);
    void onChatSelected(const QModelIndex &current, const QModelIndex &previous);
public slots:
    void onRequestsReceived(const QList<std::pair<int, QString>> &requests);
    void onContactsListReceived(const QList<Contact> &contacts);
    void onMessagesReceived(const QList<DatabaseMessage> &messages);
signals:
    void messageToSend(int contactId, const QString& text);
    void sendUuidToMainWindow(const QString& contactUuid);
    void requestForAddContactRequests();
    void sendRequestsToList(const QList<std::pair<int, QString>> &requests);
    void requestAction(int contactId, const QString &action);
    void insertMessagesInChat(const QList<DatabaseMessage> messages);
    void getMessagesForContact(int contactId);
};

#endif // MAINPAGEWIDGET_H
