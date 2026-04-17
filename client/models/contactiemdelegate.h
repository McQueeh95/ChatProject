#ifndef CONTACTIEMDELEGATE_H
#define CONTACTIEMDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QPainter>

class ContactIemDelegate: public QStyledItemDelegate
{
    Q_OBJECT
public:
    ContactIemDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;


};

#endif // CONTACTIEMDELEGATE_H
