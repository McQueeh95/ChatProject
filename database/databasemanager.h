#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include <QtSql/QSqlQuery>
#include <QUuid>
#include <QtSql/QSqlError>
#include <optional>
#include "../models/contact.h"
#include "../network/message.h"
#include "databasemessage.h"

class DataBaseManager
{
private:
    static inline QSqlDatabase db;
public:
    DataBaseManager(const QString& path);
    static bool checkConnection();
    static bool connectToDB(const QString& path);
    //Adding uuid and username to DB
    static void registerUser(const QString& uuid, const QString& username);
    //If uuid is not present in DB returns nullopt
    static std::optional<std::pair<QString, QString>> getUuid();
    static void addRequest(const QString& uuid, const QString& username, const QString& time);
    static void addRequestFromCurrent(const QString& uuid, const QString& time);
    static QList<std::pair<int, QString>> getRequests();
    static bool acceptRequest(int requestId);
    static bool rejectRequest(int requestId);
    static bool requestAccepted(const QString& uuid, const QString& username);
    static QList<Contact> getContactList();
    static QString getContactById(int id);
    static QString getRequestById(int id);
    static bool insertMessage(const Message& message);
    static QList<DatabaseMessage> getMessagesForContact(int contactId);
};

#endif // DATABASEMANAGER_H
