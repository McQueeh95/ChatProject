#include "searchviewmodel.h"

SearchViewModel::SearchViewModel(QObject *parent)
    : QAbstractListModel{parent}
{}

int SearchViewModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return m_searchRes.size();
}

QVariant SearchViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(index.row() < 0 || index.row() >= m_searchRes.size())
        return QVariant();

    const protocol::UserSearch user = m_searchRes.at(index.row());

    if(role == Qt::DisplayRole)
    {
        return user.username;
    }
    if(role == Qt::UserRole)
    {
        return user.userId;
    }
    return QVariant();
}

void SearchViewModel::setSearchRes(const QList<protocol::UserSearch> &searchRes)
{
    beginResetModel();
    m_searchRes = searchRes;
    endResetModel();
}
