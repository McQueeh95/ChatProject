#include "message.h"

Message::Message(int type, QString uuidFrom): type(type), uuidFrom(uuidFrom)
{}

Message::Message(int type, QString uuidFrom, QString uuidTo, QString text, QString time)
    :type(type), uuidFrom(uuidFrom), uuidTo(uuidTo), text(text)
{}

Message::Message(const QJsonObject &obj)
{
    type = obj["type"].toInt();
    uuidFrom = obj["uuidFrom"].toString();
    if(obj.contains("uuid_to"))
        uuidTo = obj["uuidTo"].toString();
    if(obj.contains("text"))
        text = obj["text"].toString();
    if(obj.contains("time"))
        time = obj["time"].toString();
}

QString Message::getUuidFrom() const
{
    return uuidFrom;
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
    return QJsonDocument(json).toJson(QJsonDocument::Compact);
}
