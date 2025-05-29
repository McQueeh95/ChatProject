#ifndef DATABASEMESSAGE_H
#define DATABASEMESSAGE_H

#include <QSqlQuery>

class DatabaseMessage
{
private:
    int id;
    QString content;
    QString timestamp;
public:
    DatabaseMessage();
    DatabaseMessage(const QSqlQuery &query);
    QString getContent() const;
};

#endif // DATABASEMESSAGE_H
