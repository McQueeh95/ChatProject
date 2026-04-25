#include "messagedelegate.h"
#include <QPainter>
#include <QFontMetrics>
#include <QWidget> // Обов'язково для option.widget
#include <QTextDocument>
#include <QTextOption>
#include <QListView>
#include "approles.h"

QSize MessageDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString text = index.data(Qt::DisplayRole).toString();
    int viewWidth = option.rect.width();
    if (const QListView *listView = qobject_cast<const QListView*>(option.widget)) {
        viewWidth = listView->viewport()->width();
    }
    int maxBubbleWidth = viewWidth * 0.7;

    QTextDocument doc;
    QFont font = option.font;
    font.setPointSize(11);
    doc.setDefaultFont(font);

    QTextOption textOption;
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    doc.setDefaultTextOption(textOption);

    doc.setPlainText(text);
    doc.setTextWidth(maxBubbleWidth - 20);

    int padding = 20;
    int margin = 10;

    // doc.size().height() тепер поверне ідеально точну висоту тексту!
    return QSize(viewWidth, doc.size().height() + padding + margin + 15);
}

void MessageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    QString text = index.data(Qt::DisplayRole).toString();
    bool isMyMessage = index.data(AppRoles::IsMyMessageRole).toBool();
    QString time = index.data(AppRoles::TimeRole).toString();

    int viewWidth = option.rect.width();
    if (const QListView *listView = qobject_cast<const QListView*>(option.widget)) {
        viewWidth = listView->viewport()->width();
    }
    int maxBubbleWidth = viewWidth * 0.7;

    QTextDocument doc;
    QFont font("Inter");
    font.setPointSize(11);
    doc.setDefaultFont(font);

    QTextOption textOption;
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    doc.setDefaultTextOption(textOption);

    QString htmlText = QString("<div style='color: white;'>%1</div>").arg(text.toHtmlEscaped());
    doc.setHtml(htmlText);

    doc.setTextWidth(maxBubbleWidth - 20);

    QFont timeFont("Inter");
    timeFont.setPointSize(8);
    QFontMetrics timeFm(timeFont);
    int timeWidth = timeFm.horizontalAdvance(time);

    int contentWidth = qMax((int)doc.idealWidth(), timeWidth + 10);
    int bubbleWidth = contentWidth + 20;

    int bubbleHeight = doc.size().height() + 20 + 15;

    int yPos = option.rect.top() + 5;
    QRect bubbleRect;

    if (isMyMessage) {
        bubbleRect = QRect(option.rect.right() - bubbleWidth - 15, yPos, bubbleWidth, bubbleHeight);
        painter->setBrush(QColor("#8774e1"));
    } else {
        bubbleRect = QRect(option.rect.left() + 15, yPos, bubbleWidth, bubbleHeight);
        painter->setBrush(QColor("#2c2d30"));
    }

    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(bubbleRect, 12, 12);

    QRect innerTextRect = bubbleRect.adjusted(10, 10, -10, -25);

    painter->translate(innerTextRect.topLeft());
    doc.drawContents(painter);
    painter->translate(-innerTextRect.topLeft());

    if (!time.isEmpty()) {
        painter->setFont(timeFont);

        painter->setPen(QColor(255, 255, 255, 170));

        int timeHeight = timeFm.height();

        int timeX = bubbleRect.right() - timeWidth - 10;
        int timeY = bubbleRect.bottom() - timeHeight - 5;

        painter->drawText(timeX, timeY, timeWidth, timeHeight, Qt::AlignRight | Qt::AlignVCenter, time);
    }

    painter->restore();
}
