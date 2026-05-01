#include "messagehandler.h"

MessageHandler::MessageHandler() {
    m_networkClient = new NetworkClient(this);

    m_networkClient->connectToServer("ws://127.0.0.1:8080");
    connect(m_networkClient, &NetworkClient::jsonReceived, this, &AppController::onJsonReceived);
}

void MessageHandler::onJsonReceived(const QJsonObject &obj)
{
    if(!obj.contains("type"))
    {
        qDebug() << "Json doesnt have type (onJsonReceived)";
        return;
    }
    protocol::messageType msgType = static_cast<protocol::messageType>(obj["type"].toInt());
    switch(msgType)
    {
    case protocol::messageType::SALT_RES: handleSaltRes(obj); break;
    case protocol::messageType::LOGIN_RES: handleLoginRes(obj); break;
    case protocol::messageType::REG_RES: handleRegistrationRes(obj); break;
    case protocol::messageType::FORW: handleForwardedMessage(obj); break;
    case protocol::messageType::HISTORY_RES: handleHistoryRes(obj); break;
    case protocol::messageType::DELIV_ACK: handleMessagAck(obj); break;
    case protocol::messageType::SEARCH_RES: handleSearchRes(obj); break;
    case protocol::messageType::NEW_CHAT_EVENT: handleNewChatEvent(obj); break;
    }
}

void MessageHandler::handleSaltRes(const QJsonObject &obj)
{
    protocol::SaltRes saltRes = protocol::SaltRes::fromJson(obj);
    if(!saltRes.salt.isEmpty())
    {
        emit saltReceived(saltRes.salt);
    }
    else{
        emit saltFailed();
    }
}
