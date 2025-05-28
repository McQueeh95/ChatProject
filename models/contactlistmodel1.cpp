#include "contactlistmodel1.h"

ContactListModel1::ContactListModel1(QObject *parent)
    : QAbstractListModel(parent)
{}

QVariant ContactListModel1::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

int ContactListModel1::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;
    return mContacts.size();
    // FIXME: Implement me!
}

QVariant ContactListModel1::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() < 0 || index.row() >= mContacts.size())
        return QVariant();

    const Contact &contact = mContacts.at(index.row());
    if(role == Qt::DisplayRole)
    {
        return contact.username;
    }
    if(role == Qt::UserRole)
        return contact.id;

    return QVariant();

}

void ContactListModel1::setContacts(const QList<Contact> &contacts)
{
    beginResetModel();
    mContacts = contacts;
    endResetModel();
}
