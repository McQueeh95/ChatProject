#include "contactlistmodel.h"

ContactListModel::ContactListModel(QObject *parent)
    : QAbstractItemModel(parent)
{}

QVariant ContactListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

QModelIndex ContactListModel::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

QModelIndex ContactListModel::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
}

int ContactListModel::rowCount(const QModelIndex &parent) const
{
    /*if (parent.isValid())
        return 0;*/
    qDebug() << "Size of contacts" << mContacts.size();
    return mContacts.size();
}

int ContactListModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant ContactListModel::data(const QModelIndex &index, int role) const
{
    qDebug() << "Data called! ";
    if (!index.isValid())
        return QVariant();

    if (index.row() < 0 || index.row() >= mContacts.size())
        return QVariant();

    const Contact &contact = mContacts.at(index.row());
    qDebug() << "Contact from data: " << contact.id;
    qDebug() << "data() called with index" << index.row() << "role" << role;
    if(role == Qt::DisplayRole)
    {
        return contact.username;
    }
    if(role == Qt::UserRole)
        return contact.id;

    return QVariant();
}

void ContactListModel::setContacts(const QList<Contact> &contacts)
{
    qDebug() << "Setting contacts, count:" << contacts.size();
    for (const auto &c : contacts)
        qDebug() << "  Contact:" << c.id << c.username;
    qDebug() << "Fist line of sc";
    beginResetModel();
    qDebug() << "Second line of sc";
    mContacts = contacts;
    qDebug() << "Third line of sc";
    endResetModel();
}
