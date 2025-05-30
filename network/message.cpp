#include "message.h"

/*Message::Message(int type, QString uuidFrom): type(type), uuidFrom(uuidFrom)
{}

Message::Message(int type, QString uuidFrom, QString uuidTo, QString text, QString time)
    :type(type), uuidFrom(uuidFrom), uuidTo(uuidTo), text(text)
{}*/

Message::Message(int type, const QString &uuidFrom, const QString &uuidTo, const QString &text, const QString &time, const QString &usernameFrom)
    :type(type), uuidFrom(uuidFrom), uuidTo(uuidTo), text(text), time(time), usernameFrom(usernameFrom)
{}

Message Message::createTextMessage(const QString &uuidFrom, const QString &uuidTo, const QString &text, const QString &time)
{
    return Message(0, uuidFrom, uuidTo, text, time, "");
}

Message Message::createConnectionMessage(const QString &uuidFrom)
{
    return Message(1, uuidFrom, "", "", "", "");
}

Message Message::createDisconnectMessage(const QString &uuidFrom)
{
    return Message(2, uuidFrom, "", "", "", "");
}

Message Message::createFriendRequestMessage(const QString &uuidFrom, const QString &uuidTo, const QString &time, const QString &usernameFrom)
{
    return Message(3, uuidFrom, uuidTo, "", time, usernameFrom);
}

Message Message::createContactAcceptedMessage(const QString &uuidFrom, const QString &uuidTo, const QString &time, const QString &usernameFrom)
{
    return Message(4, uuidFrom, uuidTo, "", time, usernameFrom);
}

Message Message::createContactRejectedMessage(const QString &uuidFrom, const QString &uuidTo, const QString &time)
{
    return Message(5, uuidFrom, uuidTo, "", time, "");
}



Message::Message(const QJsonObject &obj)
{
    type = obj["type"].toInt();
    uuidFrom = obj["uuid_from"].toString();
    if(obj.contains("uuid_to"))
        uuidTo = obj["uuid_to"].toString();
    if(obj.contains("text"))
        text = obj["text"].toString();
    if(obj.contains("time"))
        time = obj["time"].toString();
    if(obj.contains("username"))
        usernameFrom = obj["username"].toString();
}

int Message::getType() const
{
    return type;
}

QString Message::getUuidFrom() const
{
    return uuidFrom;
}

QString Message::getUuidTo() const
{
    return uuidTo;
}
QString Message::getText() const
{
    return text;
}

QString Message::getUsernameFrom() const
{
    return usernameFrom;
}

QString Message::getTime() const
{
    return time;
}

QString Message::toJsonString()
{
    QJsonObject json;
    json["type"] = type;
    json["uuid_from"] = uuidFrom;

    if(!uuidTo.isEmpty())
        json["uuid_to"] = uuidTo;
    if(!text.isEmpty())
        json["text"] = text;
    if(!time.isEmpty())
        json["time"] = time;
    if(!usernameFrom.isEmpty())
        json["username"] = usernameFrom;
    return QJsonDocument(json).toJson(QJsonDocument::Compact);
}
