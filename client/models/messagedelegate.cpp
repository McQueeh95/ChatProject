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
        viewWidth = listView->viewport()->width(); // Беремо точну ширину без скролбара!
    }
    int maxBubbleWidth = viewWidth * 0.7;

    // Створюємо розумний документ
    QTextDocument doc;
    QFont font = option.font;
    font.setPointSize(11);
    doc.setDefaultFont(font);

    // ВМИКАЄМО ТОЙ САМИЙ ГІБРИДНИЙ РЕЖИМ!
    QTextOption textOption;
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    doc.setDefaultTextOption(textOption);

    // Завантажуємо текст і кажемо, яка в нас максимальна ширина
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
        viewWidth = listView->viewport()->width(); // Беремо точну ширину без скролбара!
    }
    int maxBubbleWidth = viewWidth * 0.7;

    // Знову створюємо документ для малювання
    QTextDocument doc;
    QFont font = option.font;
    font.setPointSize(11);
    doc.setDefaultFont(font);

    QTextOption textOption;
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    doc.setDefaultTextOption(textOption);

    QString htmlText = QString("<div style='color: white;'>%1</div>").arg(text.toHtmlEscaped());
    doc.setHtml(htmlText);

    doc.setTextWidth(maxBubbleWidth - 20);

    // Розміри самої бульбашки беремо з документа
    QFont timeFont = option.font;
    timeFont.setPointSize(8); // Менший шрифт для часу
    QFontMetrics timeFm(timeFont);
    int timeWidth = timeFm.horizontalAdvance(time);

    // Розміри самої бульбашки
    // Беремо ширше з двох: або текст повідомлення, або текст часу
    int contentWidth = qMax((int)doc.idealWidth(), timeWidth + 10);
    int bubbleWidth = contentWidth + 20;

    // ДОДАЄМО +15 пікселів висоти для часу знизу!
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

    // 1. МАЛЮЄМО ФОН
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(bubbleRect, 12, 12);

    // 2. МАЛЮЄМО ТЕКСТ ДОКУМЕНТА
    // Знизу відступаємо більше (-25 замість -10), щоб залишити місце для часу
    QRect innerTextRect = bubbleRect.adjusted(10, 10, -10, -25);

    painter->translate(innerTextRect.topLeft());
    doc.drawContents(painter);
    // ОБОВ'ЯЗКОВО повертаємо пензлик на місце після малювання документа!
    painter->translate(-innerTextRect.topLeft());

    // 3. МАЛЮЄМО ЧАС
    if (!time.isEmpty()) {
        painter->setFont(timeFont);

        // Колір часу (робимо його напівпрозорим білим/сірим)
        painter->setPen(QColor(255, 255, 255, 170));

        int timeHeight = timeFm.height();

        // Притискаємо до правого нижнього кута бульбашки
        int timeX = bubbleRect.right() - timeWidth - 10;
        int timeY = bubbleRect.bottom() - timeHeight - 5;

        painter->drawText(timeX, timeY, timeWidth, timeHeight, Qt::AlignRight | Qt::AlignVCenter, time);
    }

    painter->restore();
}
