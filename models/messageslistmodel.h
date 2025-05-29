#ifndef MESSAGESLISTMODEL_H
#define MESSAGESLISTMODEL_H

#include <QAbstractListModel>
#include "../database/databasemessage.h"

class MessagesListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit MessagesListModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setMessages(const QList<DatabaseMessage> &messages);

private:
    QList<DatabaseMessage> mMessages;
};

#endif // MESSAGESLISTMODEL_H
