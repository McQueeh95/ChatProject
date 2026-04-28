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
public:
    NetworkClient(QObject *parent = nullptr);
    void sendJson(const QJsonObject& json);
    void connectToServer(const QUrl &url);
    bool isConnected();
private slots:
    void onTextMessageReceived(const QString& rawString);
signals:
    void jsonReceived(const QJsonObject& obj);
};

#endif // NETWORKCLIENT_H
