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
public:
    Message() = default;
    Message(int type, QString uuidFrom);
    Message(int type, QString uuidFrom, QString uuidTo, QString text, QString time);
    //Parsing from json to Message format
    Message(const QJsonObject &obj);
    QString getUuidFrom() const;
    //Parsing Message to json
    QString toJsonString();
};

#endif // MESSAGE_H
