#include "appcontroller.h"
#include <iostream>

qint64 AppController::findChatIdByUserId(qint64 targetUserId)
{
    for(const auto &c: m_chats)
    {
        if(c.peerId == targetUserId)
            return c.chatId;
    }
    return 0;
}

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

    if(m_messagesCache.contains(chatId))
    {
        emit historyReceived(chatId, m_messagesCache[chatId]);
    }
    else if(chatId > 0)
    {
        sendHistoryReq(chatId);
    }
}

void AppController::sendMessage(const QString &text)
{
    qDebug() << "localId";
    qint64 localId = QDateTime::currentMSecsSinceEpoch() * -1;

    protocol::MsgDeliv uiDraft;
    QJsonObject jsonToSend;

    if(m_currentChatId > 0)
    {
        protocol::ForwardReq forReq;
        forReq.chatId = m_currentChatId;
        forReq.localId = localId;
        forReq.payload = text;

        uiDraft.chatId = forReq.chatId;

        jsonToSend = forReq.toJson();
    }
    else if(m_phantomTargetId > 0 && m_currentChatId == 0)
    {
        qDebug() << "create and forward req";
        protocol::CreateAndForwardReq cnfReq;
        cnfReq.targetId = m_phantomTargetId;
        cnfReq.msgLocalId = localId;
        cnfReq.payload = text;
        uiDraft.chatId = (m_phantomTargetId * -1);
        jsonToSend = cnfReq.toJson();
    }

    uiDraft.localId = localId;
    uiDraft.payload = text;
    uiDraft.senderId = m_userId;
    m_messagesCache[uiDraft.chatId].push_back(uiDraft);

    if(m_currentChatId == uiDraft.chatId || m_phantomTargetId == uiDraft.chatId)
        emit localMessageCreated(uiDraft);
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

void AppController::processChatSelection(qint64 chatId, qint64 userId, const QString &username)
{
    qDebug() << "=== CHAT SELECTION ===";
    qDebug() << "1. Incoming -> chatId:" << chatId << "userId:" << userId << "username:" << username;
    if(chatId == 0 && userId > 0)
    {
        chatId = findChatIdByUserId(userId);
        qDebug() << "2. Checked existing chats. Found chatId:" << chatId;
    }
    emit chatScreenRequested(username);

    m_currentChatId = (chatId > 0) ? chatId : 0;
    m_phantomTargetId = (chatId > 0) ? 0 : userId;


    if(chatId > 0)
    {
        loadHistory(chatId);
        qDebug() << "3. Loading REAL history for chatId:" << m_currentChatId;
    }
    else if(userId > 0)
    {
        m_pendingPhantomNames.insert(userId, username);
        qDebug() << "3. Emitting EMPTY history for phantom user:" << m_phantomTargetId;
        qDebug() << "4. getCurrentChatId() returns:" << getCurrentChatId(); // ПЕРЕВІРКА!
        emit historyReceived(0, {});
        qDebug() << "Current user id" << m_phantomTargetId;
    }
    else
    {
        qDebug() << "🚨 ERROR: Both chatId and userId are 0! Model is broken!";
    }
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

            m_messagesCache[msgDeliv.chatId].push_back(msgDeliv);
            newMessageReceived(msgDeliv);
            break;
        }
        case protocol::messageType::HISTORY_RES:
        {
            protocol::HistoryRes hisRes = protocol::HistoryRes::fromJson(obj);
            qint64 chatId = hisRes.chatId;
            m_messagesCache[chatId] = hisRes.messages;
            loadHistory(chatId);
            break;
        }
        case protocol::messageType::DELIV_ACK:
        {
            qDebug() << "Deliv ack";
            protocol::DelivAck delAck = protocol::DelivAck::fromJson(obj);
            //if(!m_messagesCache.contains(delAck.chatId))
                //break;
            qDebug() << "ACK msg with local id: "<< delAck.localId;
            qDebug() << "delAck status " << delAck.status;
            if(delAck.status != "ok"){
                qDebug() << "Message delivery failed: " << delAck.errorMsg;
                return;
            }
            qDebug() << "All keys in cache:" << m_messagesCache.keys();

            if(delAck.peerId > 0)
            {
                qint64 tempChatId = (delAck.peerId * -1);
                if(m_messagesCache.contains(tempChatId))
                {
                    QList<protocol::MsgDeliv> drafts = m_messagesCache.take(tempChatId);
                    for(auto& msg : drafts)
                    {
                        msg.chatId = delAck.chatId;
                        qDebug() << "msg local id" << msg.localId;
                    }
                    m_messagesCache.insert(delAck.chatId, drafts);
                }

                protocol::ChatInfo newChat;
                newChat.chatId = delAck.chatId;
                newChat.peerId = delAck.peerId;
                newChat.peerUsername = m_pendingPhantomNames.take(newChat.peerId);

                qDebug() << "Received ACK for new chat";
                m_chats[delAck.chatId] = newChat;
                emit updateChats(newChat);
                if(m_phantomTargetId == delAck.peerId)
                {
                    m_currentChatId = delAck.chatId;
                    m_phantomTargetId = 0;
                    processChatSelection(newChat.chatId, newChat.peerId, newChat.peerUsername);
                }
            }
            QList<protocol::MsgDeliv> &messages = m_messagesCache[delAck.chatId];
            qDebug() << "2 All keys in cache:" << m_messagesCache.keys();
            for(qint64 i = messages.size() - 1; i >= 0; i--)
            {
                qDebug() << "del ack loc id" << delAck.localId;
                qDebug() << "msg[i] loc id" << delAck.localId;
                if(messages[i].localId == delAck.localId)
                {
                    messages[i].messageId = delAck.realId;
                    messages[i].timeStamp = delAck.timestamp;
                    messages[i].displayTime = delAck.displayTime;

                    if(messages[i].chatId == m_currentChatId)
                    {
                        emit msgConfirmed(messages[i]);
                    }
                    break;
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
