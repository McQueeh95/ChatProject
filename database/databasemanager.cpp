#include "databasemanager.h"

DataBaseManager::DataBaseManager(const QString& path) {
    if(QSqlDatabase::contains("qt_sql_default_connection"))
    {
        if(db.isOpen())
            return;
    }
    else
    {
        connectToDB(path);
    }
}

bool DataBaseManager::connectToDB(const QString &path)
{
    db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName(path);

    if (!db.open()) {
        qCritical() << "Was not able to open DB" << db.lastError().text();
        return false;
    }

    qDebug() << "Seccessfully connected to db";
    return true;
}

void DataBaseManager::registerUser(const QString &uuid, const QString &username)
{
    if(!db.isOpen()){
        qDebug() << "Was not able to open DB";
        return;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS user_data (uuid TEXT PRIMARY KEY, username TEXT)");

    query.prepare("INSERT INTO user_data (uuid, username) VALUES (:uuid, :username)");
    query.bindValue(":uuid", uuid);
    query.bindValue(":username", username);
    if(!query.exec())
    {
        qDebug() << "Error adding uuid and username to DB";
        return;
    }
}

std::optional<std::pair<QString, QString>> DataBaseManager::getUuid()
{
    if(!db.isOpen()){
        qDebug() << "Was not able to open DB";
        return std::nullopt;
    }
    QSqlQuery query;
    query.exec("SELECT uuid, username FROM user_data LIMIT 1");
    QString uuid;
    QString username;
    if(query.next())
    {
        uuid = query.value(0).toString();
        username = query.value(1).toString();
        qDebug() << uuid;
        return std::pair(uuid, username);
    }
    return std::nullopt;
}

void DataBaseManager::addRequest(const QString &uuid, const QString &username, const QString &time)
{
    if(!db.isOpen())
    {
        qDebug() << "Was not able to open DB";
        return;
    }

    QSqlQuery query;

    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS contact_requests ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "uuid TEXT NOT NULL,"
            "username TEXT,"
            "status TEXT NOT NULL CHECK (status IN ('pending', 'accepted', 'rejected')),"
            "timestamp TEXT"
            ")"
            ))
    {
        qCritical() << "Failed to create table:" << query.lastError().text();
    }

    query.prepare("INSERT INTO contact_requests (uuid, username, status, timestamp) VALUES (:uuid, :username, :status, :timestamp)");
    query.bindValue(":uuid", uuid);
    query.bindValue(":username", username);
    query.bindValue(":status", "pending");
    query.bindValue(":timestamp", time);
    if(!query.exec())
    {
        qCritical() << "Failed to create table:" << query.lastError().text();
        return;
    }
}

QList<std::pair<int, QString>> DataBaseManager::getRequests()
{
    QList<std::pair<int, QString>> requestes;
    if(!db.isOpen())
    {
        qDebug() << "Was not able to open DB";
        return requestes;
    }
    QSqlQuery query;
    query.exec("SELECT id, username FROM contact_requests WHERE status = 'pending'");
    while(query.next())
    {
        requestes.append({query.value(0).toInt(), query.value(1).toString()});
    }
    return requestes;
}

bool DataBaseManager::acceptRequest(int requestId)
{
    if(!db.isOpen())
    {
        qDebug() << "Was not able to open DB";
        return false;
    }
    QSqlQuery query;

    if (!query.exec(
        "CREATE TABLE IF NOT EXISTS contacts ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "uuid TEXT NOT NULL,"
        "username TEXT,"
        "timestamp TEXT);"
            ))
    {
        qCritical() << "Failed to create contacts table: " << query.lastError();
        return false;
    }

    query.prepare(
        "INSERT INTO contacts (uuid, username, timestamp) "
        "SELECT uuid, username, timestamp "
        "FROM contact_requests WHERE id = :id"
        );
    query.bindValue(":id", requestId);

    if(!query.exec())
    {
        qCritical() << "Failed to insert into contacts table: " << query.lastError();
        return false;
    }

    query.prepare("UPDATE contact_requests SET status = 'accepted' "
                  "WHERE id = :id");
    query.bindValue(":id", requestId);
    if(!query.exec())
    {
        qCritical() << "Failed to change status is contat_requests: " << query.lastError();
        return false;
    }
    return true;
}

bool DataBaseManager::rejectRequest(int requestId)
{
    if(!db.isOpen())
    {
        qDebug() << "Was not able to open DB";
        return false;
    }
    QSqlQuery query;

    query.prepare("UPDATE contact_requests SET status = 'rejected' "
                  "WHERE id = :id");
    query.bindValue(":id", requestId);
    if(!query.exec())
    {
        qCritical() << "Failed to insert into contacts table: " << query.lastError();
        return false;
    }
    return true;
}

QList<Contact> DataBaseManager::getContactList()
{
    QList<Contact> contacts;
    if(!db.isOpen())
    {
        qDebug() << "Was not able to open DB";
        return contacts;
    }
    QSqlQuery query;

    if(!query.exec("SELECT id, username FROM contacts"))
    {
        qCritical() << "Failed to insert into contacts table: " << query.lastError();
        return contacts;
    }
    while(query.next())
    {
        Contact contact;
        contact.id = query.value(0).toInt();
        contact.username = query.value(1).toString();
        contacts.push_back(contact);
    }
    return contacts;
}

QString DataBaseManager::getContactById(int id)
{
    QString contactUuid;
    if(!db.isOpen())
    {
        qDebug() << "Was not able to open DB";
        return contactUuid;
    }

    QSqlQuery query;

    query.prepare("SELECT uuid FROM contacts WHERE id = :id");
    query.bindValue(":id", id);
    if(!query.exec())
    {
        qCritical() << "Failed to insert into contacts table: " << query.lastError();
        return contactUuid;
    }

    while (query.next())
    {
        contactUuid = query.value(0).toString();
        qDebug() << "Found UUID:" << contactUuid;
    }

    return contactUuid;

}

