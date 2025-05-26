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

bool NetworkClient::sendMessage(const QString &msgText, const QString &uuidTo)
{
    if(mWebSocket.state() == QAbstractSocket::ConnectedState)
    {
        QString timeStamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
        Message msg(0, uuid, uuidTo, msgText, timeStamp);
        QString toSend = msg.toJsonString();
        mWebSocket.sendTextMessage(toSend);
        return true;
    }
    else
    {
        qDebug() << "Error sending message";
        return false;
    }
}

void NetworkClient::onConnected()
{
    connect(&mWebSocket, &QWebSocket::textMessageReceived, this, &NetworkClient::onMessageReceived);
    Message msg(1, this->uuid);
    QString jsonString = msg.toJsonString();
    mWebSocket.sendTextMessage(jsonString);
}

void NetworkClient::onMessageReceived()
{

}

void NetworkClient::onDisconnected()
{
    qDebug() << "WebSocket disconnected";
}
