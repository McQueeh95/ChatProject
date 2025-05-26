#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include <QtSql/QSqlQuery>
#include <QUuid>
#include <QtSql/QSqlError>
#include <optional>

class DataBaseManager
{
private:
    static inline QSqlDatabase db;
public:
    DataBaseManager(const QString& path);
    static bool connectToDB(const QString& path);
    //Adding uuid and username to DB
    static void registerUser(const QString& uuid, const QString& username);
    //If uuid is not present in DB returns nullopt
    static std::optional<QString> getUuid();
};

#endif // DATABASEMANAGER_H
