#ifndef CHATVIEWMODEL_H
#define CHATVIEWMODEL_H

#include <QAbstractListModel>
#include "../types/networktypes.h"
#include "../types/uitypes.h"
//#include "ui_contactmodelwidget.h"

class ChatViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ChatViewModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setChats(const QList<UiStruct::ChatPreview> &chats);

    void appendChat(const UiStruct::ChatPreview &chat);

    void clear();


private:
    QList<UiStruct::ChatPreview> m_chats;
};

#endif // CHATVIEWMODEL_H
