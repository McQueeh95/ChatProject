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
    query.prepare("INSERT INTO contact_requests (uuid, username, status, timestamp) VALUES (:uuid, :username, :status, :timestamp)");
    query.bindValue(":uuid", uuid);
    query.bindValue(":username", username);
    query.bindValue(":status", "pending");
    query.bindValue(":timestamp", time);
    if(!query.exec())
    {
        qDebug() << "Error adding uuid and username to DB";
        return;
    }
}

QList<QString> DataBaseManager::getRequests()
{
    QList<QString> requestes;
    if(!db.isOpen())
    {
        qDebug() << "Was not able to open DB";
        return requestes;
    }
    QSqlQuery query;
    query.exec("SELECT username FROM contact_requests WHERE status = 'pending'");
    while(query.next())
    {
        requestes.append(query.value(0).toString());
    }
    return requestes;
}

