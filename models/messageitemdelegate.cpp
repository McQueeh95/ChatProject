    #include "messageitemdelegate.h"


    MessageItemDelegate::MessageItemDelegate(const QString &localUuid, QObject *parent)
        :QStyledItemDelegate(parent), mLocalUuid(localUuid)
    {}

    void MessageItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        painter->save();

        QString text = index.data(Qt::DisplayRole).toString();
        QString senderUuid = index.data(Qt::UserRole).toString();

        bool isOwnMessage = (senderUuid == mLocalUuid);

        QRect rect = option.rect.adjusted(10, 5, -10, -5);
        QFontMetrics fm(option.font);
        int textWidth = rect.width();
        QRect textRect = fm.boundingRect(QRect(0, 0, textWidth, 0), Qt::TextWordWrap, text);
        textRect.setHeight(textRect.height() + 15);
        textRect.setWidth(textRect.width() + 10);

        QRect messageRect;
        if (isOwnMessage)
            messageRect = QRect(rect.right() - textRect.width(), rect.top(), textRect.width(), textRect.height());
        else
            messageRect = QRect(rect.left(), rect.top(), textRect.width(), textRect.height());
        QFont font = option.font;
        //font.setPixelSize(14);
        painter->setFont(font);
        QColor bubbleColor = isOwnMessage ? QColor("#dcf8c6") : QColor("#ffffff");
        painter->setBrush(bubbleColor);
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(messageRect, 12, 12);

        painter->setPen(Qt::black);
        painter->drawText(messageRect.adjusted(5, 5, -5, -5), Qt::TextWordWrap, text);

        painter->restore();
    }

    QSize MessageItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        int maxBubbleWidth = 300;
        QString text = index.data(Qt::DisplayRole).toString();
        QFont font = option.font;
        QFontMetrics fm(font);
        int textWidth = option.rect.width();
        int height = fm.boundingRect(QRect(0, 0, textWidth, 0), Qt::TextWordWrap, text).height();
        qDebug() << "paint rect width:" << option.rect.width();
        qDebug() << "sizeHint width:" << option.rect.width();
        return QSize(option.rect.width(), height + 30);
    }
