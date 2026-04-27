#include "chatdelegate.h"

QSize ChatDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(option.rect.width(), 60);
}

void ChatDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    QString username = index.data(Qt::DisplayRole).toString();

    //Background
    if(option.state & QStyle::State_Selected)
    {
        painter->setBrush(QColor("#3D3D3D"));
        painter->setPen(Qt::NoPen);

        painter->drawRoundedRect(option.rect.adjusted(-2, -2, -2, -2), 8, 8);
    }
    else if (option.state & QStyle::State_MouseOver)
    {
        painter->setBrush(QColor("#2E2E2E"));
        painter->setPen(Qt::NoPen);

        painter->drawRoundedRect(option.rect.adjusted(-2, -2, -2, -2), 8, 8);
    }

    int margin = 10;
    int avatarSize = 40;

    int avatartY = option.rect.top() + (option.rect.height() - avatarSize) / 2;

    QRect avatarRect(option.rect.left() + margin, avatartY, avatarSize, avatarSize);

    int textX = avatarRect.right() + margin;
    QRect textRect(textX, option.rect.top(), option.rect.width() - textX - margin, option.rect.height());

    //Avatar
    painter->setBrush(QColor("#8774e1"));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(avatarRect);

    painter->setPen(Qt::white);
    QFont boldFont("Inter");
    boldFont.setBold(true);
    boldFont.setPointSize(13);
    painter->setFont(boldFont);

    QString initial = username.isEmpty() ? "?" : username.left(1).toUpper();
    painter->drawText(avatarRect, Qt::AlignCenter, initial);

    //Username
    QFont nameFont("Inter");
    painter->setPen(QColor("#ffffff"));
    nameFont.setPointSize(12);
    nameFont.setBold(false);
    painter->setFont(nameFont);

    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, username);

    painter->restore();

}
