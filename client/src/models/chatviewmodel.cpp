#include "chatviewmodel.h"
#include <QSize>
#include "approles.h"

ChatViewModel::ChatViewModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int ChatViewModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_chats.size();
}

QVariant ChatViewModel::data(const QModelIndex &index, int role) const
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

void ChatViewModel::setChats(const QList<UiStruct::ChatPreview> &chats)
{
    beginResetModel();
    m_chats = chats;
    endResetModel();
}

void ChatViewModel::clear()
{
    beginResetModel();

    m_chats.clear();

    endResetModel();
}
