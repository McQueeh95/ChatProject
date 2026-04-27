#include "contactiemdelegate.h"

//ContactIemDelegate::ContactIemDelegate() {}

ContactIemDelegate::ContactIemDelegate(QObject *parent)
    :QStyledItemDelegate(parent)
{}

void ContactIemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    if (option.state & QStyle::State_Selected) {
        painter->setBrush(QColor("#1A1A1A"));
        painter->setPen(Qt::NoPen);
        painter->drawRect(option.rect);
    }
    else if (option.state & QStyle::State_MouseOver) {
        painter->setBrush(QColor("#212121"));
        painter->setPen(Qt::NoPen);
        painter->drawRect(option.rect);
    }



    QString username = index.data(Qt::DisplayRole).toString();

    QRect rect = option.rect;

    int margin = 10;
    int avatarSize = rect.height() - 2 * margin;
    QRect avatarRect(rect.left() + margin, rect.top() + margin, avatarSize, avatarSize);

    painter->setBrush(Qt::gray);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(avatarRect);

    painter->setPen(Qt::white);
    QFont boldFont = option.font;
    boldFont.setBold(true);
    painter->setFont(boldFont);

    QString initials = username.left(1).toUpper();
    painter->drawText(avatarRect, Qt::AlignCenter, initials);

    int textX = avatarRect.right() + margin;
    QRect nameRect(textX, rect.top() + margin, rect.width() - textX - margin, rect.height() / 2 + margin);

    painter->setPen(Qt::white);
    QFont nameFont = option.font;
    nameFont.setPointSize(14);
    nameFont.setBold(true);
    painter->setFont(nameFont);
    painter->drawText(nameRect, Qt::AlignVCenter | Qt::AlignLeft, username);

    painter->restore();



}

QSize ContactIemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QSize(0, 60);
}
