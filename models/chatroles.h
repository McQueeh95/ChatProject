#ifndef CHATROLES_H
#define CHATROLES_H
#include <Qt>

namespace ChatRoles
{
    enum MessageRoles{
        MessageIdRole = Qt::UserRole + 1,
        TimeRole = Qt::UserRole + 2,
        StatusRole = Qt::UserRole + 3
    };
}

#endif // CHATROLES_H
