#include "appcontroller.h"
#include <iostream>

AppController::AppController() {
    m_networkClient = new NetworkClient(this);

    m_networkClient->connectToServer(QUrl("ws://127.0.0.1:8080"));
    connect(m_networkClient, &NetworkClient::jsonReceived, this, &AppController::onJsonReceived);
}

void AppController::loginUser(const QString& username, const QString& password)
{
    protocol::LoginReq loginReq;
    loginReq.username = username;
    loginReq.hashedPassword = password;

    QJsonObject jsonToSend = loginReq.toJson();

    if(m_networkClient == nullptr)
    {
        return;
    }
    m_networkClient->sendJson(jsonToSend);
}

void AppController::createUser(const QString& username, const QString& password)
{
    protocol::RegisterReq regReq;
    regReq.username = username;
    regReq.hashedPassword = password;
    QJsonObject jsonToSend = regReq.toJson();

    if(m_networkClient == nullptr)
        return;

    m_networkClient->sendJson(jsonToSend);
}

void AppController::loadHistory(qint64 chatId)
{
    this->m_currentChatId = chatId;

    if(m_chatsCache.contains(chatId))
    {
        emit historyReceived(chatId, m_chatsCache[chatId]);
    }
    else
    {
        sendHistoryReq(chatId);
    }
}

void AppController::sendMessage(const QString &text)
{
    if(m_currentChatId == -1) return;

    qint64 localId = QDateTime::currentMSecsSinceEpoch() * -1;

    protocol::ForwardReq forReq;
    forReq.chatId = m_currentChatId;
    forReq.localId = localId;
    forReq.payload = text;

    protocol::MsgDeliv uiDraft;
    uiDraft.localId = localId;
    uiDraft.payload = text;
    uiDraft.chatId = m_currentChatId;
    uiDraft.senderId = m_userId;

    m_chatsCache[uiDraft.chatId].push_back(uiDraft);

    emit localMessageCreated(uiDraft);

    QJsonObject jsonToSend = forReq.toJson();
    this->m_networkClient->sendJson(jsonToSend);
    qDebug() << "send message with local id " << localId;
}

void AppController::sendHistoryReq(qint64 chatId)
{
    protocol::HistoryReq hisReq;
    hisReq.chatId = chatId;
    QJsonObject jsonToSend = hisReq.toJson();
    this->m_networkClient->sendJson(jsonToSend);
}

void AppController::searchUsers(const QString &query)
{
    qDebug() << "searchUsers start";
    protocol::SearchReq searchReq;
    searchReq.toFind = query;

    QJsonObject jsonToSend = searchReq.toJson();
    this->m_networkClient->sendJson(jsonToSend);
    qDebug() << "searchUsers sent";
}

qint64 AppController::getCurrentChatId()
{
    return m_currentChatId;
}

void AppController::onJsonReceived(const QJsonObject& obj)
{
    if(!obj.contains("type"))
    {
        qDebug() << "Json doesnt have type (onJsonReceived)";
        return;
    }
    protocol::messageType msgType = static_cast<protocol::messageType>(obj["type"].toInt());
    switch(msgType)
    {
        case protocol::messageType::LOGIN_RES:
        {
            protocol::LoginRes loginRes = protocol::LoginRes::fromJson(obj);
            if(loginRes.status == "ok")
            {
                this->m_userId = loginRes.userId;

                this->m_chats = loginRes.chats;

                QList<protocol::ChatInfo> chatsList = m_chats.values();

                std::sort(chatsList.begin(), chatsList.end(), [](const protocol::ChatInfo& a, const protocol::ChatInfo& b)
                          {return a.peerUsername < b.peerUsername;});

                emit loginSuccess(m_userId, chatsList);
            }
            else
            {
                emit loginFailure();
            }
            break;
        }
        case protocol::messageType::REG_RES:
        {
            protocol::RegRes regRes = protocol::RegRes::fromJson(obj);
            if(regRes.status == "ok")
            {
                this->m_userId = regRes.userId;

                emit registrationSuccess(m_userId);
            }
            break;
        }
        case protocol::messageType::FORW:
        {
            protocol::MsgDeliv msgDeliv = protocol::MsgDeliv::fromJson(obj);

            m_chatsCache[msgDeliv.chatId].push_back(msgDeliv);
            newMessageReceived(msgDeliv);
            break;
        }
        case protocol::messageType::HISTORY_RES:
        {
            protocol::HistoryRes hisRes = protocol::HistoryRes::fromJson(obj);
            qint64 chatId = hisRes.chatId;
            m_chatsCache[chatId] = hisRes.messages;
            loadHistory(chatId);
            break;
        }
        case protocol::messageType::DELIV_ACK:
        {
            qDebug() << "Deliv ack";
            protocol::DelivAck delAck = protocol::DelivAck::fromJson(obj);
            //if(!m_chatsCache.contains(delAck.chatId))
                //break;
            qDebug() << "ACK msg with local id: "<< delAck.localId;
            qDebug() << "delAck status " << delAck.status;
            if(delAck.status == "ok")
            {
                QList<protocol::MsgDeliv> &messages = m_chatsCache[delAck.chatId];
                qDebug() << "Status ok";
                for(qint64 i = messages.size() - 1; i > 0; i--)
                {
                    if(messages[i].localId == delAck.localId)
                    {
                        messages[i].messageId = delAck.realId;
                        messages[i].timeStamp = delAck.timestamp;
                        messages[i].displayTime = delAck.displayTime;
                        qDebug() << "TimeStamp " << delAck.timestamp;
                        qDebug() << "DisplayTime i " << messages[i].displayTime;
                        qDebug() << "DisplayTime delACk " << delAck.displayTime;

                        if(messages[i].chatId == m_currentChatId)
                        {
                            emit msgConfirmed(messages[i]);
                        }
                        break;
                    }
                }
            }
            break;
        }
        case protocol::messageType::SEARCH_RES:
        {
            qDebug() << "case searchres";
            protocol::SearchRes searchRes = protocol::SearchRes::fromJson(obj);
            for(auto u: searchRes.foundUsers)
            {
                qDebug() << "found: " << u.username;
            }
            emit foundUsers(searchRes.foundUsers);
        }
    }
}
