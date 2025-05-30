#ifndef MESSAGE_H
#define MESSAGE_H
#include <QString>
#include <QJsonObject>

class Message
{
private:
    int type;
    QString uuidFrom;
    QString uuidTo;
    QString text;
    QString time;
    QString usernameFrom;
    Message(int type, const QString &uuidFrom, const QString &uuidTo, const QString &text, const QString &time, const QString &usernameFrom);
public:
    Message() = default;
    static Message createTextMessage(const QString &uuidFrom, const QString &uuidTo, const QString &text, const QString &time);
    static Message createConnectionMessage(const QString &uuidFrom);
    static Message createDisconnectMessage(const QString &uuidFrom);
    static Message createFriendRequestMessage(const QString &uuidFrom, const QString &uuidTo, const QString &time, const QString &usernameFrom);
    static Message createContactAcceptedMessage(const QString &uuidFrom, const QString &uuidTo, const QString &time, const QString &usernameFrom);
    static Message createContactRejectedMessage(const QString &uuidFrom, const QString &uuidTo, const QString &time, const QString &usernameFrom);
    //Message(int type, QString uuidFrom);
    //Message(int type, QString uuidFrom, QString uuidTo, QString text, QString time);
    //Message(int type, QString uuidFrom, QString uuidTo, QString usernameFrom, QString time);
    //Parsing from json to Message format
    Message(const QJsonObject &obj);
    int getType() const;
    QString getUuidFrom() const;
    QString getUuidTo() const;
    QString getText() const;
    QString getUsernameFrom() const;
    QString getTime() const;
    //Parsing Message to json
    QString toJsonString();
};

#endif // MESSAGE_H
