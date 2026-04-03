#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include "network/networkclient.h"
#include "networktypes.h"

class AppController: public QObject
{
    Q_OBJECT
private:
    NetworkClient* m_networkClient;
    QHash<qint64, protocol::ChatInfo> m_chats;
    qint64 m_userId;
    qint64 m_currentChatId = 0;
    qint64 m_phantomTargetId = 0;
    QHash<qint64, QList<protocol::MsgDeliv>> m_messagesCache;
    QHash<qint64, QString> m_pendingPhantomNames;
    qint64 findChatIdByUserId(qint64 targetUserId);
public:
    AppController();
    void loginUser(const QString& username, const QString& password);
    void createUser(const QString& username, const QString& password);
    void loadHistory(qint64 chatId);
    void sendMessage(const QString &text);
    void sendHistoryReq(qint64 chatId);
    void searchUsers(const QString &query);
    qint64 getCurrentChatId();
    void processChatSelection(qint64 chatId, qint64 userId, const QString& username);
private slots:
    void onJsonReceived(const QJsonObject& obj);
signals:
    void loginSuccess(qint64 userId, QList<protocol::ChatInfo> chatsList);
    void loginFailure();
    void registrationSuccess(qint64 userId);
    void historyReceived(qint64 chatId, QList<protocol::MsgDeliv> messagesList);
    void newMessageReceived(const protocol::MsgDeliv &msg);
    void localMessageCreated(const protocol::MsgDeliv &msg);
    void msgConfirmed(const protocol::MsgDeliv &msg);
    void foundUsers(const QList<protocol::UserSearch> &users);
    void chatScreenRequested(const QString &username);
    void updateChats(const protocol::ChatInfo &chat);
    void phantomChatResolved(qint64 newChatId);
};

#endif // APPCONTROLLER_H
