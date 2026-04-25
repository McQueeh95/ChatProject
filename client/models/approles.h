#ifndef APPROLES_H
#define APPROLES_H
#include <Qt>

namespace AppRoles
{
    enum MessageRoles{
        MessageIdRole = Qt::UserRole + 1,
        TimeRole = Qt::UserRole + 2,
        StatusRole = Qt::UserRole + 3,
        IsMyMessageRole = Qt::UserRole + 4,
        ShowDateRole = Qt::UserRole + 5,
        DateRole = Qt::UserRole + 6
    };

    enum ChatRoles
    {
        ChatIdRole = Qt::UserRole + 100,
        UserIdRole = Qt::UserRole + 200,
    };
}

#endif // APPROLES_H
