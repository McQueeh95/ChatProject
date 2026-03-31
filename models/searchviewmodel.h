#ifndef SEARCHVIEWMODEL_H
#define SEARCHVIEWMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QWidget>
#include "../networktypes.h"

class SearchViewModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SearchViewModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    void setSearchRes(const QList<protocol::UserSearch> &searchRes);

private:
    QList<protocol::UserSearch> m_searchRes;


};

#endif // SEARCHVIEWMODEL_H
