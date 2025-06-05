#include "databasemessage.h"

DatabaseMessage::DatabaseMessage() {}

DatabaseMessage::DatabaseMessage(const QSqlQuery &query)
{
    this->id = query.value(0).toInt();
    this->senderUuid = query.value(1).toString();
    this->content = query.value(2).toString();
    this->timestamp = query.value(3).toString();
}

QString DatabaseMessage::getContent() const
{
    return this->content;
}

QString DatabaseMessage::getSenderUuid() const
{
    return this->senderUuid;
}
