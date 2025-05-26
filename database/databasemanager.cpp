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

std::optional<QString> DataBaseManager::getUuid()
{
    if(!db.isOpen()){
        qDebug() << "Was not able to open DB";
        return std::nullopt;
    }
    QSqlQuery query;
    query.exec("SELECT uuid FROM user_data LIMIT 1");
    QString uuid;
    if(query.next())
    {
        uuid = query.value(0).toString();
        qDebug() << uuid;
        return uuid;
    }
    return std::nullopt;
}

