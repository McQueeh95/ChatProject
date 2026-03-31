#ifndef MESSAGEVIEWMODEL_H
#define MESSAGEVIEWMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include "../networktypes.h"

class MessageViewModel : public QAbstractListModel
{
public:
    explicit MessageViewModel(QObject *parent = nullptr);



    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    void setMessages(const QList<protocol::MsgDeliv> &messages);
    void setUserId(qint64 userId);

    void appendMessage(const protocol::MsgDeliv &msg);
    void updateMessage(const protocol::MsgDeliv &msg);

private:
    QList<protocol::MsgDeliv> m_messages;
    qint64 m_userId = 0;
};

#endif // MESSAGEVIEWMODEL_H
