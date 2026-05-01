#include "networkclient.h"
#include <QDateTime>

NetworkClient::NetworkClient(const QUrl &serverUrl, QObject *parent) : QObject(parent)
{
    m_webSocket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);

    m_serverUrl = serverUrl;
    connectToServer();
    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setInterval(5000);

    connect(m_reconnectTimer, &QTimer::timeout, this, &NetworkClient::connectToServer);
    connect(m_webSocket, &QWebSocket::textMessageReceived, this, &NetworkClient::onTextMessageReceived);
    connect(m_webSocket, &QWebSocket::disconnected, this, &NetworkClient::onDisconnected);
    connect(m_webSocket, &QWebSocket::connected, this, &NetworkClient::onConnected);
}

void NetworkClient::sendJson(const QJsonObject& json)
{
    if(m_webSocket->state() == QAbstractSocket::ConnectedState && !json.empty())
    {
        QString toSend = QJsonDocument(json).toJson(QJsonDocument::Compact);
        m_webSocket->sendTextMessage(toSend);
    }
    else
        qDebug() << "Error sending Json(send json)";
}

void NetworkClient::connectToServer()
{
    qDebug() << "Trying to connect";
    m_webSocket->open(m_serverUrl);
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

void NetworkClient::onDisconnected()
{
    m_reconnectTimer->start();
    emit connectionLost();
}

void NetworkClient::onConnected()
{
    m_reconnectTimer->stop();
    emit connectionRestored();
}

bool NetworkClient::isConnected()
{
    return(m_webSocket->state() == QAbstractSocket::ConnectedState);
}
