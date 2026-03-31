#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H
#include <QWebSocket>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>

class NetworkClient : public QObject
{
    Q_OBJECT
private:
    QWebSocket m_webSocket;
    QString uuid;
    QString username;
public:
    NetworkClient(QObject *parent = nullptr);
    void sendJson(const QJsonObject& json);
    void connectToServer(const QUrl &url);
    bool isConnected();
    void setUsername(const QString &username);
    void disconnectFromServer();
private slots:
    void onDisconnected();
    void onTextMessageReceived(const QString& rawString);
    void onConnected();
signals:
    void connected();
    void jsonReceived(const QJsonObject& obj);
};

#endif // NETWORKCLIENT_H
