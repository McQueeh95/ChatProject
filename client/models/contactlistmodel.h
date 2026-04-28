#ifndef CONTACTLISTMODEL_H
#define CONTACTLISTMODEL_H

#include <QAbstractListModel>
#include "../networktypes.h"
//#include "ui_contactmodelwidget.h"

class ContactListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ContactListModel(QObject *parent = nullptr);

    // Header:
    //QVariant headerData(int section,
                        //Qt::Orientation orientation,
                        //int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setChats(const QList<protocol::ChatInfo> &chats);

    void appendChat(const protocol::ChatInfo &chat);

    void clear();


private:
    QList<protocol::ChatInfo> m_chats;
};

#endif // CONTACTLISTMODEL_H
