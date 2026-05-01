#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H
#include <QWebSocket>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

class NetworkClient : public QObject
{
    Q_OBJECT
private:
    QWebSocket *m_webSocket;
    QTimer *m_reconnectTimer;
    QUrl m_serverUrl;
public:
    NetworkClient(const QUrl &serverUrl, QObject *parent = nullptr);
    void sendJson(const QJsonObject& json);
    bool isConnected();

private slots:
    void onTextMessageReceived(const QString& rawString);
    void onDisconnected();
    void onConnected();
    void connectToServer();
signals:
    void jsonReceived(const QJsonObject& obj);
    void connectionLost();
    void connectionRestored();
};

#endif // NETWORKCLIENT_H
