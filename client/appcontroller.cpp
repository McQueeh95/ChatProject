#include "appcontroller.h"
#include <iostream>
#include <QDateTime>

AppController::AppController() {
    m_networkClient = new NetworkClient(this);
    m_cryptoService = new CryptoService(this);

    m_networkClient->connectToServer(QUrl("ws://127.0.0.1:8080"));
    connect(m_networkClient, &NetworkClient::jsonReceived, this, &AppController::onJsonReceived);
}

void AppController::requestSalt(const QString& username, const QString& password)
{
    m_pendingPassword.load(password);
    m_username = username;

    protocol::SaltReq saltReq;

    saltReq.username = username;
    QJsonObject jsonToSend = saltReq.toJson();

    if(m_networkClient != nullptr)
        m_networkClient->sendJson(jsonToSend);
}

void AppController::createUser(const QString& username, const QString& password)
{
    m_username = username;

    RegistrationData generatedData = m_cryptoService->generateNewAccount(password);

    protocol::RegisterReq regReq;
    regReq.username = username;
    regReq.authKey = generatedData.authKey;
    regReq.salt = generatedData.salt;
    regReq.publicKey = generatedData.publicKey;
    regReq.encryptedVault = generatedData.encryptedVault;
    regReq.vaultNonce = generatedData.vaultNonce;

    QJsonObject jsonToSend = regReq.toJson();

    if(m_networkClient != nullptr)
        m_networkClient->sendJson(jsonToSend);
}

void AppController::loadHistory(qint64 chatId)
{
    //Chat exist in cache no need to ask server
    if(m_messagesCache.contains(chatId))
    {
        emit historyReceived(chatId, m_messagesCache[chatId]);
    }
    //Chat doesn't exist ask server
    else if(chatId > 0)
    {
        sendHistoryReq(chatId);
    }
}

void AppController::processChatSelection(qint64 chatId, qint64 userId, const QString &username)
{
    emit chatScreenRequested(username);
    if(chatId == 0 && userId > 0)
    {
        chatId = findChatIdByUserId(userId);
    }

    m_currentChatId = (chatId > 0) ? chatId : (userId * (-1));
    if(m_currentChatId > 0)
    {
        loadHistory(m_currentChatId);
    }
    else if(m_currentChatId < 0)
    {
        UiStruct::PhantomChat phantom;
        phantom.tempChatId = (userId * -1);
        phantom.username = username;
        phantom.publicKey = m_searchCache[userId].publicKey;

        m_pendingPhantoms.insert(userId, phantom);
        emit noMessagesYet(username);
    }
}

void AppController::sendMessage(const QString &text)
{
    qint64 localId = QDateTime::currentMSecsSinceEpoch() * -1;

    UiStruct::Message uiDraft = makeUiDraft(localId, text);

    processLocalMessage(uiDraft);

    QJsonObject jsonToSend = makeMessageJson(localId, text);
    m_networkClient->sendJson(jsonToSend);
}

void AppController::sendHistoryReq(qint64 chatId)
{
    protocol::HistoryReq hisReq;
    hisReq.chatId = chatId;
    QJsonObject jsonToSend = hisReq.toJson();
    m_networkClient->sendJson(jsonToSend);
}

void AppController::searchUsers(const QString &query)
{
    protocol::SearchReq searchReq;
    searchReq.toFind = query;

    QJsonObject jsonToSend = searchReq.toJson();
    m_networkClient->sendJson(jsonToSend);
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

qint64 AppController::findChatIdByUserId(qint64 targetUserId)
{
    for(const auto &c: m_chats)
    {
        if(c.peerId == targetUserId)
            return c.chatId;
    }
    return 0;
}

QJsonObject AppController::makeMessageJson(qint64 localId, const QString &text)
{
    if(m_currentChatId > 0)
    {
        EncryptedMessage msg = m_cryptoService->encryptMessage(text, m_chats[m_currentChatId].publicKey);
        protocol::ForwardReq forReq;
        forReq.chatId = m_currentChatId;
        forReq.localId = localId;
        forReq.payload = msg.cipherText;
        forReq.nonce = msg.nonce;

        return forReq.toJson();
    }
    else if(m_currentChatId < 0)
    {
        protocol::StartChatReq startChatReq;
        startChatReq.targetId = (m_currentChatId * -1);
        startChatReq.msgLocalId = localId;

        EncryptedMessage msg = m_cryptoService->encryptMessage(text, m_pendingPhantoms[startChatReq.targetId].publicKey);

        startChatReq.payload = msg.cipherText;
        startChatReq.nonce = msg.nonce;

        return startChatReq.toJson();
    }
}

UiStruct::Message AppController::makeUiDraft(qint64 localId, const QString &text)
{
    UiStruct::Message uiDraft;
    uiDraft.chatId = m_currentChatId;
    uiDraft.localId = localId;
    uiDraft.text = text;
    uiDraft.senderId = m_userId;
    return uiDraft;
}

void AppController::processLocalMessage(const UiStruct::Message &message)
{
    //Add message to chat to show in the UI
    m_messagesCache[message.chatId].push_back(message);

    //If chat still opened
    if(m_currentChatId == message.chatId)
        emit localMessageCreated(message);
}

void AppController::handleSaltRes(const QJsonObject &obj)
{
    protocol::SaltRes saltRes = protocol::SaltRes::fromJson(obj);
    if(!saltRes.salt.isEmpty())
    {
        DerivedKeys keys = m_cryptoService->generateHashedPassword(m_pendingPassword.dataChar(), m_pendingPassword.size(), saltRes.salt);
        m_pendingLocalKey.load(keys.localEncryptKey);

        protocol::LoginReq loginReq;
        loginReq.username = m_username;
        loginReq.authKey = keys.authKey;

        QJsonObject jsonToSend = loginReq.toJson();

        m_networkClient->sendJson(jsonToSend);
    }
    else{
        emit loginFailure();
    }
}

void AppController::handleLoginRes(const QJsonObject &obj)
{
    protocol::LoginRes loginRes = protocol::LoginRes::fromJson(obj);
    if(loginRes.status == "ok")
    {
        protocol::UserInfo user = loginRes.userInfo;

        m_userId = user.userId;
        m_chats = loginRes.chats;

        QList<protocol::ChatInfo> chatsList = m_chats.values();
        for(const auto &c: chatsList)
        {
            qDebug() << c.publicKey.toBase64();
        }
        std::sort(chatsList.begin(), chatsList.end(), [](const protocol::ChatInfo& a, const protocol::ChatInfo& b)
                  {return a.peerUsername < b.peerUsername;});

        m_cryptoService->decryptSecretKey(user.encryptedVault, user.vaultNonce, m_pendingLocalKey.dataUCHar());
        emit loginSuccess(m_userId, chatsList);
    }
    else
        emit loginFailure();
}

void AppController::handleRegistrationRes(const QJsonObject &obj)
{
    protocol::RegRes regRes = protocol::RegRes::fromJson(obj);
    if(regRes.status == "ok")
    {
        m_userId = regRes.userId;

        emit registrationSuccess(m_userId);
    }
    else
        emit registrationFailure();
}

void AppController::handleForwardedMessage(const QJsonObject &obj)
{
    protocol::MsgDeliv msgDeliv = protocol::MsgDeliv::fromJson(obj);
    QByteArray chatPublicKey = m_chats[msgDeliv.chatId].publicKey;
    QString decryptedText = m_cryptoService->decryptMessage(msgDeliv.payload,
                                chatPublicKey, msgDeliv.nonce);
    UiStruct::Message uiMsg = UiStruct::Message::fromNetwork(msgDeliv, decryptedText);
    m_messagesCache[msgDeliv.chatId].push_back(uiMsg);
    newMessageReceived(uiMsg);
}

void AppController::handleHistoryRes(const QJsonObject &obj)
{
    protocol::HistoryRes hisRes = protocol::HistoryRes::fromJson(obj);
    qint64 chatId = hisRes.chatId;
    QByteArray chatPublicKey = m_chats[chatId].publicKey;

    QList<UiStruct::Message> uiMessages;

    for(const auto &netMsg: hisRes.messages)
    {
        QString decryptedText = m_cryptoService->decryptMessage(
            netMsg.payload, chatPublicKey, netMsg.nonce);

        uiMessages.append(UiStruct::Message::fromNetwork(netMsg, decryptedText));
    }

    m_messagesCache[chatId] = uiMessages;
    loadHistory(chatId);
}

void AppController::handleSearchRes(const QJsonObject &obj)
{
    protocol::SearchRes searchRes = protocol::SearchRes::fromJson(obj);
    m_searchCache.clear();
    for(const auto &s: searchRes.foundUsers)
    {
        m_searchCache.insert(s.userId, s);
    }
    emit foundUsers(searchRes.foundUsers);
}

void AppController::handleMessagAck(const QJsonObject &obj)
{
    protocol::DelivAck delAck = protocol::DelivAck::fromJson(obj);
    if(delAck.status != "ok"){
        qDebug() << "Message delivery failed: " << delAck.errorMsg;
        return;
    }

    if(delAck.peerId > 0)
    {
        promotePhantomChat(delAck);
    }

    confirmDeliveryMessage(delAck);
}

void AppController::handleNewChatEvent(const QJsonObject &obj)
{
    protocol::ChatInfo newChat = protocol::ChatInfo::fromJson(obj);
    m_chats[newChat.chatId] = newChat;
    emit updateChats(newChat);
}

void AppController::promotePhantomChat(const protocol::DelivAck &delAck)
{
    qint64 tempChatId = (delAck.peerId * -1);
    if(m_messagesCache.contains(tempChatId))
    {
        QList<UiStruct::Message> drafts = m_messagesCache.take(tempChatId);
        for(auto& msg : drafts)
        {
            msg.chatId = delAck.chatId;
        }
        m_messagesCache.insert(delAck.chatId, drafts);
    }
    protocol::ChatInfo newChat;
    UiStruct::PhantomChat phantom = m_pendingPhantoms.take(newChat.peerId);
    newChat.chatId = delAck.chatId;
    newChat.peerId = delAck.peerId;
    newChat.peerUsername = phantom.username;
    newChat.publicKey = phantom.publicKey;

    if(m_pendingPhantoms.contains(delAck.peerId))
    {
        newChat.publicKey = m_pendingPhantoms.value(delAck.peerId).publicKey;
    }
    else
    {
        qDebug() << "User was not found in search cache";
    }

    qDebug() << newChat.publicKey.toBase64();
    m_chats[delAck.chatId] = newChat;
    emit updateChats(newChat);
    if(m_currentChatId == (delAck.peerId * -1))
    {
        m_currentChatId = delAck.chatId;

        processChatSelection(newChat.chatId, newChat.peerId, newChat.peerUsername);
    }
}

void AppController::confirmDeliveryMessage(const protocol::DelivAck &delAck)
{
    if(!m_messagesCache.contains(delAck.chatId)) return;

    QList<UiStruct::Message> &messages = m_messagesCache[delAck.chatId];
    for(qint64 i = messages.size() - 1; i >= 0; i--)
    {
        if(messages[i].localId == delAck.localId)
        {
            messages[i].messageId = delAck.realId;
            messages[i].displayTime = UiStruct::Message::formatTime(delAck.timestamp);

            if(messages[i].chatId == m_currentChatId)
            {
                emit msgConfirmed(messages[i]);
            }
            break;
        }
    }
}
