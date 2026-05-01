#include "messageviewmodel.h"
#include "approles.h"

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

    const UiStruct::Message message = m_messages.at(index.row());

    if(role == Qt::DisplayRole)
    {
        return message.text;
    }
    if(role == AppRoles::MessageIdRole)
    {
        return message.messageId;
    }
    if(role == AppRoles::TimeRole)
    {
        if(message.messageId == 0 || message.displayTime.isEmpty())
        {
            return "sending...";
        }
        return message.displayTime;
    }
    if(role == AppRoles::IsMyMessageRole)
    {
        return message.senderId == m_userId;
    }
    if(role == AppRoles::ShowDateRole)
    {
        return message.showDate;
    }
    if(role == AppRoles::DateRole)
    {
        return message.displayDate;
    }

    return QVariant();
}


void MessageViewModel::setMessages(const QList<UiStruct::Message> &messages)
{
    beginResetModel();
    m_messages = messages;

    QString lastDate = "";
    for(int i = 0; i < m_messages.size(); i++)
    {
        if(m_messages[i].displayDate != lastDate)
        {
            m_messages[i].showDate = true;
            lastDate = m_messages[i].displayDate;
        }
        else
        {
            m_messages[i].showDate = false;
        }
    }

    endResetModel();
}

void MessageViewModel::setUserId(qint64 userId)
{
    this->m_userId = userId;
}

void MessageViewModel::appendMessage(UiStruct::Message msg)
{
    int newRow = m_messages.size();
    beginInsertRows(QModelIndex(), newRow, newRow);

    if(!m_messages.isEmpty())
    {
        QString lastDate = m_messages.last().displayDate;

        if(msg.displayDate != lastDate)
        {
            msg.showDate = true;
        }
        else
        {
            msg.showDate = false;
        }
    }
    else
    {
        msg.showDate = true;
    }
    m_messages.append(msg);

    endInsertRows();
}

void MessageViewModel::updateMessage(const UiStruct::Message &msg)
{
    for(qint64 i = m_messages.size() - 1; i >= 0; i--)
    {
        if(m_messages[i].localId == msg.localId)
        {
            m_messages[i].displayTime = msg.displayTime;
            m_messages[i].displayDate = msg.displayDate;
            m_messages[i].messageId = msg.messageId;

            QModelIndex idx = index(i, 0);

            emit dataChanged(idx, idx);

            break;
        }
    }
}

void MessageViewModel::clearMessages()
{
    beginResetModel();
    m_messages.clear();
    endResetModel();
}
