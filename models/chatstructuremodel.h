#ifndef CHATSTRUCTUREMODEL_H
#define CHATSTRUCTUREMODEL_H

#include <QAbstractListModel>

class chatStructureModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit chatStructureModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
};

#endif // CHATSTRUCTUREMODEL_H
