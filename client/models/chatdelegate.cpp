#include "chatdelegate.h"
#include "approles.h"

QSize ChatDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(option.rect.width(), 60);
}

void ChatDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    QString username = index.data(Qt::DisplayRole).toString();

    QString lastMessage = index.data(AppRoles::LastMsgRole).toString();
    QString displayTime = index.data(AppRoles::DateTimeRole).toString();

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

    int textX = avatarRect.right() + margin;
    int contentWidth = option.rect.width() - textX - margin;

    //Username
    QFont nameFont("Inter");
    nameFont.setPointSize(12);
    nameFont.setBold(false);

    QFont msgFont("Inter");
    msgFont.setPointSize(11);

    QFont timeFont("Inter");
    timeFont.setPointSize(10);

    QFontMetrics nameFm(nameFont);
    QFontMetrics msgFm(msgFont);
    QFontMetrics timeFm(timeFont);

    int textSpacing = 2;
    int totalTextHeight = nameFm.height() + textSpacing + msgFm.height();
    int startY = option.rect.top() + (option.rect.height() - totalTextHeight) / 2;

    QRect topRowRect(textX, startY, contentWidth, nameFm.height());
    QRect bottomRowRect(textX, topRowRect.bottom() + textSpacing, contentWidth, msgFm.height());

    painter->setFont(timeFont);
    painter->setPen(QColor("#A0A0A0"));
    int timeWidth = timeFm.horizontalAdvance(displayTime);
    QRect timeRect(topRowRect.right() - timeWidth, topRowRect.top(), timeWidth, topRowRect.height());
    painter->drawText(timeRect, Qt::AlignRight | Qt::AlignVCenter, displayTime);

    painter->setFont(nameFont);
    painter->setPen(QColor("#FFFFFF"));
    QRect nameRect = topRowRect;
    nameRect.setWidth(contentWidth - timeWidth - margin);
    QString elidedName = nameFm.elidedText(username, Qt::ElideRight, nameRect.width());
    painter->drawText(nameRect, Qt::AlignLeft | Qt::AlignVCenter, elidedName);

    painter->setFont(msgFont);
    painter->setPen(QColor("#888888"));
    QString elidedMsg = msgFm.elidedText(lastMessage, Qt::ElideRight, bottomRowRect.width());
    painter->drawText(bottomRowRect, Qt::AlignLeft | Qt::AlignVCenter, elidedMsg);

    painter->restore();
}
