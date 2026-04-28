#include "contactlistmodel.h"
#include <QSize>
#include "approles.h"

ContactListModel::ContactListModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int ContactListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;
    return m_chats.size();
    // FIXME: Implement me!
}

QVariant ContactListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() < 0 || index.row() >= m_chats.size())
        return QVariant();

    const protocol::ChatInfo &chat = m_chats.at(index.row());
    if(role == Qt::DisplayRole)
    {
        return chat.peerUsername;
    }
    if(role == AppRoles::ChatIdRole)
    {
        return chat.chatId;
    }

    return QVariant();

}

void ContactListModel::setChats(const QList<protocol::ChatInfo> &chats)
{
    beginResetModel();
    m_chats = chats;
    endResetModel();
}

void ContactListModel::appendChat(const protocol::ChatInfo &chat)
{
    int newRow = m_chats.size();
    beginInsertRows(QModelIndex(), newRow, newRow);

    m_chats.append(chat);

    endInsertRows();
}

void ContactListModel::clear()
{
    beginResetModel();

    m_chats.clear();

    endResetModel();
}
