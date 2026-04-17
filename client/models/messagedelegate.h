#ifndef MESSAGEDELEGATE_H
#define MESSAGEDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>

class MessageDelegate : public QStyledItemDelegate
{
public:
    explicit MessageDelegate(QObject *parent = nullptr):  QStyledItemDelegate(parent){}
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // MESSAGEDELEGATE_H
