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
}

QVariant ContactListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() < 0 || index.row() >= m_chats.size())
        return QVariant();

    const UiStruct::ChatPreview &chat = m_chats.at(index.row());
    if(role == Qt::DisplayRole)
    {
        return chat.peerUsername;
    }
    if(role == AppRoles::ChatIdRole)
    {
        return chat.chatId;
    }
    if(role == AppRoles::LastMsgRole)
    {
        return chat.text;
    }
    if(role == AppRoles::DateTimeRole)
    {
        return chat.displayDateTime;
    }

    return QVariant();

}

void ContactListModel::setChats(const QList<UiStruct::ChatPreview> &chats)
{
    beginResetModel();
    m_chats = chats;
    endResetModel();
}

void ContactListModel::clear()
{
    beginResetModel();

    m_chats.clear();

    endResetModel();
}
