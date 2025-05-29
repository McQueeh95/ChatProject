#ifndef CONTACTLISTMODEL_H
#define CONTACTLISTMODEL_H

#include <QAbstractListModel>
#include "contact.h"

class ContactListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ContactListModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setContacts(const QList<Contact> &contacts);

private:
    QList<Contact> mContacts;
};

#endif // CONTACTLISTMODEL_H
