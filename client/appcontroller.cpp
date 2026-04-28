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
    m_session.reset(new SessionState());

    m_session->pendingPassword.load(password);
    m_session->username = username;

    protocol::SaltReq saltReq;

    saltReq.username = username;
    QJsonObject jsonToSend = saltReq.toJson();


    if(m_networkClient != nullptr)
        m_networkClient->sendJson(jsonToSend);
    qDebug() << "reques Salt sent json";
}

void AppController::createUser(const QString& username, const QString& password)
{
    m_session.reset(new SessionState());

    m_session->username = username;

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
    if(!m_session) return;

    //Chat exist in cache no need to ask server
    if(m_session->messagesCache.contains(chatId))
    {
        emit historyReceived(chatId, m_session->messagesCache[chatId]);
    }
    //Chat doesn't exist ask server
    else if(chatId > 0)
    {
        sendHistoryReq(chatId);
    }
}

void AppController::processChatSelection(qint64 chatId, qint64 userId, const QString &username)
{
    if(!m_session) return;

    emit chatScreenRequested(username);
    if(chatId == 0 && userId > 0)
    {
        chatId = findChatIdByUserId(userId);
    }

    m_session->currentChatId = (chatId > 0) ? chatId : (userId * (-1));
    if(m_session->currentChatId > 0)
    {
        loadHistory(m_session->currentChatId);
    }
    else if(m_session->currentChatId < 0)
    {
        UiStruct::PhantomChat phantom;
        phantom.tempChatId = (userId * -1);
        phantom.username = username;
        phantom.publicKey = m_session->searchCache[userId].publicKey;

        m_session->pendingPhantoms.insert(userId, phantom);
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

void AppController::logout()
{
    m_cryptoService->clearSecretKey();

    QJsonObject jsonToSend = protocol::LogoutReq::toJson();
    m_networkClient->sendJson(jsonToSend);

    if(m_session)
        m_session.reset();

    emit performLogout();
}

qint64 AppController::getCurrentChatId()
{
    if(m_session)
        return m_session->currentChatId;
    return 0;
}

QString AppController::getUsername()
{
    if(m_session)
        return m_session->username;
    return QString();
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
    if(!m_session)
        return 0;

    for(const auto &c: m_session->chats)
    {
        if(c.peerId == targetUserId)
            return c.chatId;
    }
    return 0;
}

QJsonObject AppController::makeMessageJson(qint64 localId, const QString &text)
{
    if(!m_session)
        return {};

    if(m_session->currentChatId > 0)
    {
        EncryptedMessage msg = m_cryptoService->encryptMessage(text, m_session->chats[m_session->currentChatId].publicKey);
        protocol::ForwardReq forReq;
        forReq.chatId = m_session->currentChatId;
        forReq.localId = localId;
        forReq.payload = msg.cipherText;
        forReq.nonce = msg.nonce;

        return forReq.toJson();
    }
    else if(m_session->currentChatId < 0)
    {
        protocol::StartChatReq startChatReq;
        startChatReq.targetId = -m_session->currentChatId;
        startChatReq.msgLocalId = localId;

        EncryptedMessage msg = m_cryptoService->encryptMessage(text, m_session->pendingPhantoms[startChatReq.targetId].publicKey);

        startChatReq.payload = msg.cipherText;
        startChatReq.nonce = msg.nonce;

        return startChatReq.toJson();
    }

    return {};
}

UiStruct::Message AppController::makeUiDraft(qint64 localId, const QString &text)
{
    if(!m_session)
        return{};

    UiStruct::Message uiDraft;
    //uiDraft.chatId = m_currentChatId;
    uiDraft.chatId = m_session->currentChatId;
    uiDraft.localId = localId;
    uiDraft.text = text;
    uiDraft.senderId = m_session->userId;
    uiDraft.displayDate = QDateTime::currentDateTime().toString("dd.MM.yyyy");
    return uiDraft;
}

void AppController::processLocalMessage(const UiStruct::Message &message)
{
    if(!m_session)
        return;

    //Add message to chat to show in the UI
    m_session->messagesCache[message.chatId].push_back(message);

    //If chat still opened
    if(m_session->currentChatId == message.chatId)
        emit localMessageCreated(message);
}

void AppController::handleSaltRes(const QJsonObject &obj)
{
    if(!m_session)
        return;

    protocol::SaltRes saltRes = protocol::SaltRes::fromJson(obj);
    if(!saltRes.salt.isEmpty())
    {
        DerivedKeys keys = m_cryptoService->generateHashedPassword(m_session->pendingPassword.dataChar(),
                                                                   m_session->pendingPassword.size(), saltRes.salt);
        m_session->pendingLocalKey.load(keys.localEncryptKey);

        protocol::LoginReq loginReq;
        loginReq.username = m_session->username;
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
    if(!m_session)
        return;

    protocol::LoginRes loginRes = protocol::LoginRes::fromJson(obj);
    qDebug() << "started login";
    if(loginRes.status == "ok")
    {
        protocol::UserInfo user = loginRes.userInfo;

        m_session->userId = user.userId;
        m_session->chats = loginRes.chats;

        QList<protocol::ChatInfo> chatsList = m_session->chats.values();
        for(const auto &c: chatsList)
        {
            qDebug() << c.publicKey.toBase64();
        }
        std::sort(chatsList.begin(), chatsList.end(), [](const protocol::ChatInfo& a, const protocol::ChatInfo& b)
                  {return a.peerUsername < b.peerUsername;});

        m_cryptoService->decryptSecretKey(user.encryptedVault, user.vaultNonce, m_session->pendingLocalKey.dataUCHar());
        qDebug() << "decrypted secret key";
        emit loginSuccess(m_session->userId, chatsList, m_session->username);
    }
    else
        emit loginFailure();
}

void AppController::handleRegistrationRes(const QJsonObject &obj)
{
    if(!m_session)
        return;

    protocol::RegRes regRes = protocol::RegRes::fromJson(obj);
    if(regRes.status == "ok")
    {
        m_session->userId = regRes.userId;

        emit registrationSuccess(m_session->userId, m_session->username);
    }
    else
        emit registrationFailure();
}

void AppController::handleForwardedMessage(const QJsonObject &obj)
{
    if(!m_session)
        return;

    protocol::MsgDeliv msgDeliv = protocol::MsgDeliv::fromJson(obj);

    QByteArray chatPublicKey = m_session->chats[msgDeliv.chatId].publicKey;
    QString decryptedText = m_cryptoService->decryptMessage(msgDeliv.payload,
                                chatPublicKey, msgDeliv.nonce);

    UiStruct::Message uiMsg = UiStruct::Message::fromNetwork(msgDeliv, decryptedText);
    m_session->messagesCache[msgDeliv.chatId].push_back(uiMsg);
    if(m_session->currentChatId == msgDeliv.chatId)
        newMessageReceived(uiMsg);
}

void AppController::handleHistoryRes(const QJsonObject &obj)
{
    if(!m_session)
        return;

    protocol::HistoryRes hisRes = protocol::HistoryRes::fromJson(obj);
    qint64 chatId = hisRes.chatId;
    QByteArray chatPublicKey = m_session->chats[chatId].publicKey;

    QList<UiStruct::Message> uiMessages;

    for(const auto &netMsg: hisRes.messages)
    {
        QString decryptedText = m_cryptoService->decryptMessage(
            netMsg.payload, chatPublicKey, netMsg.nonce);

        uiMessages.append(UiStruct::Message::fromNetwork(netMsg, decryptedText));
    }

    m_session->messagesCache[chatId] = uiMessages;
    loadHistory(chatId);
}

void AppController::handleSearchRes(const QJsonObject &obj)
{
    if(!m_session)
        return;

    protocol::SearchRes searchRes = protocol::SearchRes::fromJson(obj);
    m_session->searchCache.clear();
    for(const auto &s: searchRes.foundUsers)
    {
        m_session->searchCache.insert(s.userId, s);
    }
    emit foundUsers(searchRes.foundUsers);
}

void AppController::handleMessagAck(const QJsonObject &obj)
{
    if(!m_session)
        return;

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
    if(!m_session)
        return;

    protocol::ChatInfo newChat = protocol::ChatInfo::fromJson(obj);
    m_session->chats[newChat.chatId] = newChat;
    emit updateChats(newChat);
}

void AppController::promotePhantomChat(const protocol::DelivAck &delAck)
{
    if(!m_session)
        return;

    qint64 tempChatId = -delAck.peerId;

    if(m_session->messagesCache.contains(tempChatId))
    {
        QList<UiStruct::Message> drafts = m_session->messagesCache.take(tempChatId);
        for(auto& msg : drafts)
        {
            msg.chatId = delAck.chatId;
        }
        m_session->messagesCache.insert(delAck.chatId, drafts);
    }

    if (!m_session->pendingPhantoms.contains(delAck.peerId)) {
        qDebug() << "Critical: Phantom chat for peer" << delAck.peerId << "was not found!";
        return;
    }
    UiStruct::PhantomChat phantom = m_session->pendingPhantoms.take(delAck.peerId);

    protocol::ChatInfo newChat;
    newChat.chatId = delAck.chatId;
    newChat.peerId = delAck.peerId;
    newChat.peerUsername = phantom.username;
    newChat.publicKey = phantom.publicKey;

    m_session->chats[delAck.chatId] = newChat;
    emit updateChats(newChat);
    if(m_session->currentChatId == tempChatId)
    {
        m_session->currentChatId = delAck.chatId;
        processChatSelection(newChat.chatId, newChat.peerId, newChat.peerUsername);
    }
}

void AppController::confirmDeliveryMessage(const protocol::DelivAck &delAck)
{
    if(!m_session)
        return;

    if(!m_session->messagesCache.contains(delAck.chatId)) return;

    QList<UiStruct::Message> &messages = m_session->messagesCache[delAck.chatId];
    for(qint64 i = messages.size() - 1; i >= 0; i--)
    {
        if(messages[i].localId == delAck.localId)
        {
            messages[i].messageId = delAck.realId;
            messages[i].displayTime = UiStruct::Message::formatTime(delAck.timestamp);

            if(messages[i].chatId == m_session->currentChatId)
            {
                emit msgConfirmed(messages[i]);
            }
            break;
        }
    }
}
