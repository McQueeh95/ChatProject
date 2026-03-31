#ifndef MESSAGESLISTMODEL_H
#define MESSAGESLISTMODEL_H

#include <QAbstractListModel>

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


    void setUuid(const QString &localUuid);

private:
    QString localUuid;
};

#endif // MESSAGESLISTMODEL_H
