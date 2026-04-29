#ifndef CONTACTLISTMODEL_H
#define CONTACTLISTMODEL_H

#include <QAbstractListModel>
#include "../networktypes.h"
#include "../uitypes.h"
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

    void setChats(const QList<UiStruct::ChatPreview> &chats);

    void appendChat(const UiStruct::ChatPreview &chat);

    void clear();


private:
    QList<UiStruct::ChatPreview> m_chats;
};

#endif // CONTACTLISTMODEL_H
