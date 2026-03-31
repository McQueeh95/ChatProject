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
    qint64 m_currentChatId = -1;
    QHash<qint64, QList<protocol::MsgDeliv>> m_chatsCache;
public:
    AppController();
    void loginUser(const QString& username, const QString& password);
    void createUser(const QString& username, const QString& password);
    void loadHistory(qint64 chatId);
    void sendMessage(const QString &text);
    void sendHistoryReq(qint64 chatId);
    void searchUsers(const QString &query);
    qint64 getCurrentChatId();
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
};

#endif // APPCONTROLLER_H
