#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include "network/networkclient.h"
#include "networktypes.h"
#include "cryptoservice.h"
#include "securebuffer.h"
#include "uitypes.h"

struct SessionState
{
    qint64 userId = 0;
    qint64 currentChatId = 0;
    qint64 phantomTargetId = 0;
    QString username;
    QHash<qint64, protocol::ChatInfo> chats;
    QHash<qint64, QList<UiStruct::Message>> messagesCache;
    QHash<qint64, UiStruct::PhantomChat> pendingPhantoms;
    QHash<qint64, protocol::UserSearch> searchCache;
    SecureBuffer pendingPassword;
    SecureBuffer pendingLocalKey;
};

class AppController: public QObject
{
    Q_OBJECT

public:
    AppController();

    void requestSalt(const QString& username, const QString& password);
    void createUser(const QString& username, const QString& password);

    void loadHistory(qint64 chatId);
    void processChatSelection(qint64 chatId, qint64 userId, const QString& username);
    void sendMessage(const QString &text);
    void sendHistoryReq(qint64 chatId);
    void searchUsers(const QString &query);
    void logout();

    qint64 getCurrentChatId();
    QString getUsername();

signals:
    void loginSuccess(qint64 userId, QList<protocol::ChatInfo> chatsList, const QString& username);
    void loginFailure();
    void registrationSuccess(qint64 userId, const QString &username);
    void registrationFailure();
    void performLogout();

    void historyReceived(qint64 chatId, QList<UiStruct::Message> messagesList);
    void newMessageReceived(const UiStruct::Message &msg);
    void localMessageCreated(const UiStruct::Message &msg);
    void msgConfirmed(const UiStruct::Message &msg);
    void foundUsers(const QList<protocol::UserSearch> &users);
    void chatScreenRequested(const QString &username);
    void noMessagesYet(const QString &username);
    void updateChats(const protocol::ChatInfo &chat);
    void phantomChatResolved(qint64 newChatId);

private slots:
    void onJsonReceived(const QJsonObject& obj);

private:
    //Helpers
    qint64 findChatIdByUserId(qint64 targetUserId);
    QJsonObject makeMessageJson(qint64 localId, const QString &text);
    UiStruct::Message makeUiDraft(qint64 localId, const QString &text);
    void processLocalMessage(const UiStruct::Message &message);

    //Handles server's responses
    void handleSaltRes (const QJsonObject &obj);
    void handleLoginRes(const QJsonObject &obj);
    void handleRegistrationRes(const QJsonObject &obj);
    void handleForwardedMessage(const QJsonObject &obj);
    void handleHistoryRes(const QJsonObject &obj);
    void handleSearchRes(const QJsonObject &obj);
    void handleMessagAck(const QJsonObject &obj);
    void handleNewChatEvent(const QJsonObject &obj);
    void promotePhantomChat(const protocol::DelivAck &delAck);
    void confirmDeliveryMessage(const protocol::DelivAck &delAck);
    UiStruct::Message mapToUi(const protocol::MsgDeliv &netMsg, const QByteArray &peerPublicKey);

    CryptoService* m_cryptoService;
    NetworkClient* m_networkClient;

    QScopedPointer<SessionState> m_session;
    /*SecureBuffer m_pendingPassword;
    SecureBuffer m_pendingLocalKey;
    QHash<qint64, protocol::ChatInfo> m_chats;
    QString m_username;
    qint64 m_userId = 0;
    qint64 m_currentChatId = 0;
    qint64 m_phantomTargetId = 0;
    QHash<qint64, QList<UiStruct::Message>> m_messagesCache;
    QHash<qint64, UiStruct::PhantomChat> m_pendingPhantoms;
    QHash<qint64, protocol::UserSearch> m_searchCache;*/
};

#endif // APPCONTROLLER_H
