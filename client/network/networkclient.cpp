#include "networkclient.h"
#include <QDateTime>

NetworkClient::NetworkClient(QObject *parent) : QObject(parent)
{
    //connect(&m_webSocket, &QWebSocket::disconnected, this, &NetworkClient::onDisconnected);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &NetworkClient::onTextMessageReceived);
}

void NetworkClient::sendJson(const QJsonObject& json)
{
    if(m_webSocket.state() == QAbstractSocket::ConnectedState && !json.empty())
    {
        QString toSend = QJsonDocument(json).toJson(QJsonDocument::Compact);
        m_webSocket.sendTextMessage(toSend);
    }
    else
        qDebug() << "Error sending Json(send json)";
}

void NetworkClient::connectToServer(const QUrl &url)
{
    m_webSocket.open(url);
}

void NetworkClient::onTextMessageReceived(const QString& rawString)
{
    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(rawString.toUtf8());
    if(!doc.isNull())
    {
        if(doc.isObject())
        {
            obj = doc.object();
        }
        else
        {
            qDebug() << "Json is not an object (onTextMessageReceived)";
            return;
        }
    }
    else
    {
        qDebug() << "Invalid Json (onTextMessageReceived)";
    }
    emit jsonReceived(obj);
}

bool NetworkClient::isConnected()
{
    return(m_webSocket.state() == QAbstractSocket::ConnectedState);
}
