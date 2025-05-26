#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H
#include <QWebSocket>
#include <QUrl>

class NetworkClient : public QObject
{
private:
    QWebSocket mWebSocket;
    QString uuid;
public:
    NetworkClient(const QUrl &url, QObject *parent = nullptr);
    void setUuid(const QString &uuid);
    bool sendMessage(const QString &msgText, const QString &uuidTo);
private slots:
    void onConnected();
    void onMessageReceived();
    void onDisconnected();
};

#endif // NETWORKCLIENT_H
