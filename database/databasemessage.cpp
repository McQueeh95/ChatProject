#include "databasemessage.h"

DatabaseMessage::DatabaseMessage() {}

DatabaseMessage::DatabaseMessage(const QSqlQuery &query)
{
    this->id = query.value(0).toInt();
    this->content = query.value(1).toString();
    this->timestamp = query.value(2).toString();
}

QString DatabaseMessage::getContent() const
{
    return this->content;
}
