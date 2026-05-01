#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QObject>
#include "network/networkclient.h"
#include "networktypes.h"

class MessageHandler
{
public:
    MessageHandler();
signals:
    saltFailed();
private slots:
    void onJsonReceived(const QJsonObject& obj);
private:
    void handleSaltRes (const QJsonObject &obj);
    void handleLoginRes(const QJsonObject &obj);
    void handleRegistrationRes(const QJsonObject &obj);
    void handleForwardedMessage(const QJsonObject &obj);
    void handleHistoryRes(const QJsonObject &obj);
    void handleSearchRes(const QJsonObject &obj);
    void handleMessagAck(const QJsonObject &obj);
    void handleNewChatEvent(const QJsonObject &obj);
    void handleDeliveryAck(const protocol::DelivAck &delAck);

    NetworkClient* m_networkClient;
};

#endif // MESSAGEHANDLER_H
