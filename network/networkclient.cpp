#include "networkclient.h"
#include "message.h"
#include <QDateTime>

NetworkClient::NetworkClient(const QUrl &url, QObject *parent) : QObject(parent)
{
    connect(&mWebSocket, &QWebSocket::connected, this, &NetworkClient::onConnected);
    connect(&mWebSocket, &QWebSocket::disconnected, this, &NetworkClient::onDisconnected);
    mWebSocket.open(url);
}

void NetworkClient::setUuid(const QString &uuid)
{
    this->uuid = uuid;
}

void NetworkClient::setUsername(const QString &username)
{
    this->username = username;
}

bool NetworkClient::sendMessage(const QString &msgText, const QString &uuidTo)
{
    if(mWebSocket.state() == QAbstractSocket::ConnectedState)
    {
        QString timeStamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
        Message msg = Message::createTextMessage(uuid, uuidTo, msgText, timeStamp);
        QString toSend = msg.toJsonString();
        mWebSocket.sendTextMessage(toSend);
        //emit addContactRequestSent(msg);
        return true;
    }
    else
    {
        qDebug() << "Error sending message";
        return false;
    }
}

bool NetworkClient::sendAddContactRequest(const QString &uuidTo)
{
    if(mWebSocket.state() == QAbstractSocket::ConnectedState)
    {
        QString timeStamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
        Message msg = Message::createFriendRequestMessage(this->uuid, uuidTo, timeStamp, this->username);
        qDebug() << uuidTo << " " << msg.getUuidFrom() << " " << msg.getUsernameFrom();
        QString jsonString = msg.toJsonString();
        qDebug() << jsonString;
        mWebSocket.sendTextMessage(jsonString);
        emit addContactRequestSent(msg);
        return true;
    }
    return false;
}

bool NetworkClient::sendContactAccepted(const QString &uuidTo)
{
    if(mWebSocket.state() == QAbstractSocket::ConnectedState)
    {
        QString timeStamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
        Message msg = Message::createContactAcceptedMessage(this->uuid, uuidTo, timeStamp, this->getUsername());
        QString jsonString = msg.toJsonString();
        mWebSocket.sendTextMessage(jsonString);
        return true;
    }
    return false;
}

void NetworkClient::disconnectFromServer()
{
    Message message = Message::createDisconnectMessage(this->uuid);
    QString toSend = message.toJsonString();
    mWebSocket.sendTextMessage(toSend);
    mWebSocket.close();
}

QString NetworkClient::getUuid() const
{
    return uuid;
}

QString NetworkClient::getUsername() const
{
    return username;
}

void NetworkClient::onConnected()
{
    connect(&mWebSocket, &QWebSocket::textMessageReceived, this, &NetworkClient::onMessageReceived);

    Message msg = Message::createConnectionMessage(this->uuid);
    QString jsonString = msg.toJsonString();
    mWebSocket.sendTextMessage(jsonString);
}

void NetworkClient::onMessageReceived(const QString &message)
{
    QJsonObject obj = QJsonDocument::fromJson(message.toUtf8()).object();
    Message msg(obj);
    emit messageReceived(msg);
}

void NetworkClient::onDisconnected()
{
    qDebug() << "WebSocket disconnected";
}
