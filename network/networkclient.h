#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H
#include <QWebSocket>
#include <QUrl>
#include "message.h"

class NetworkClient : public QObject
{
    Q_OBJECT
private:
    QWebSocket mWebSocket;
    QString uuid;
    QString username;
public:
    NetworkClient(const QUrl &url, QObject *parent = nullptr);
    void setUuid(const QString &uuid);
    void setUsername(const QString &username);
    bool sendMessage(const QString &msgText, const QString &uuidTo);
    bool sendAddContactRequest(const QString &uuidTo);
    void disconnectFromServer();
private slots:
    void onConnected();
    void onMessageReceived(const QString &message);
    void onDisconnected();
signals:
    Message messageReceived(const Message &message);
};

#endif // NETWORKCLIENT_H
