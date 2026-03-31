#include "messageviewmodel.h"
#include "chatroles.h"

MessageViewModel::MessageViewModel(QObject *parent)
    : QAbstractListModel{parent}
{}

int MessageViewModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_messages.size();
}

QVariant MessageViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(index.row() < 0 || index.row() >= m_messages.size())
        return QVariant();

    const protocol::MsgDeliv message = m_messages.at(index.row());

    if(role == Qt::DisplayRole)
    {
        return message.payload;
    }
    if(role == ChatRoles::MessageIdRole)
    {
        return message.messageId;
    }
    if(role == ChatRoles::TimeRole)
    {
        if(message.messageId == 0 || message.timeStamp.isEmpty())
        {
            return "sending...";
        }
        return message.displayTime;
    }
    if(role == Qt::UserRole)
    {
        return message.senderId == m_userId;
    }

    return QVariant();
}


void MessageViewModel::setMessages(const QList<protocol::MsgDeliv> &messages)
{
    beginResetModel();
    m_messages = messages;
    endResetModel();
}

void MessageViewModel::setUserId(qint64 userId)
{
    this->m_userId = userId;
}

void MessageViewModel::appendMessage(const protocol::MsgDeliv &msg)
{
    int newRow = m_messages.size();
    beginInsertRows(QModelIndex(), newRow, newRow);

    m_messages.append(msg);

    endInsertRows();
}

void MessageViewModel::updateMessage(const protocol::MsgDeliv &msg)
{
    beginResetModel();
    for(qint64 i = m_messages.size() - 1; i >= 0; i--)
    {
        if(m_messages[i].localId == msg.localId)
        {
            m_messages[i].timeStamp = msg.timeStamp;
            m_messages[i].displayTime = msg.displayTime;
            m_messages[i].messageId = msg.messageId;
        }
    }
    endResetModel();
}
