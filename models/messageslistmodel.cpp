#include "messageslistmodel.h"

MessagesListModel::MessagesListModel(QObject *parent)
    : QAbstractListModel(parent)
{}

QVariant MessagesListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

int MessagesListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;
    return mMessages.size();

    // FIXME: Implement me!
}

QVariant MessagesListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(index.row() < 0 || index.row() >= mMessages.size())
        return QVariant();

    const DatabaseMessage &message = mMessages.at(index.row());

    if(role == Qt::DisplayRole)
    {
        return message.getContent();
    }
    if(role == Qt::UserRole)
    {
        return message.getSenderUuid();
    }

    return QVariant();
}

void MessagesListModel::setMessages(const QList<DatabaseMessage> &messages)
{
    beginResetModel();
    mMessages = messages;
    endResetModel();
}

void MessagesListModel::setUuid(const QString &localUuid)
{
    this->localUuid = localUuid;
}
