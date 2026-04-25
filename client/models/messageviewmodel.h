#ifndef MESSAGEVIEWMODEL_H
#define MESSAGEVIEWMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include "../uitypes.h"

class MessageViewModel : public QAbstractListModel
{
public:
    explicit MessageViewModel(QObject *parent = nullptr);



    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    void setMessages(const QList<UiStruct::Message> &messages);
    void setUserId(qint64 userId);

    void appendMessage(UiStruct::Message msg);
    void updateMessage(const UiStruct::Message &msg);
    void clearMessages();

private:
    QList<UiStruct::Message> m_messages;
    qint64 m_userId = 0;
};

#endif // MESSAGEVIEWMODEL_H
