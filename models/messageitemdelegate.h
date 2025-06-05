#ifndef MESSAGEITEMDELEGATE_H
#define MESSAGEITEMDELEGATE_H
#include <QStyledItemDelegate>
#include <QPainter>

class MessageItemDelegate: public QStyledItemDelegate
{
    Q_OBJECT
public:
    MessageItemDelegate(const QString &localUuid, QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
private:
    QString mLocalUuid;
};

#endif // MESSAGEITEMDELEGATE_H
