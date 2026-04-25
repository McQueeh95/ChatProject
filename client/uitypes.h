#ifndef UITYPES_H
#define UITYPES_H
#include "networktypes.h"

namespace UiStruct {
    struct Message
    {
        qint64 messageId = 0;
        qint64 localId = 0;
        qint64 chatId = 0;
        qint64 senderId = 0;
        QString text;
        QString displayTime;
        QString displayDate;
        bool showDate;

        static QString formatTime(const QString &isoTime)
        {
            QDateTime dt = QDateTime::fromString(isoTime, Qt::ISODate);
            if(dt.isValid())
            {
                return dt.toString("HH:mm");
            }
            else
            {
                qDebug() << "dt is not valid:" << isoTime;
                return isoTime;
            }
        }

        static QString formatDate(const QString &isoTime)
        {
            QDateTime dt = QDateTime::fromString(isoTime, Qt::ISODate);
            if(dt.isValid())
            {
                return dt.toString("dd.MM.yyyy");
            }
            else
            {
                qDebug() << "dt is not valid:" << isoTime;
                return "";
            }
        }

        static Message fromNetwork(const protocol::MsgDeliv &netMsg, const QString &decryptedText)
        {
            Message uiMsg;
            uiMsg.messageId = netMsg.messageId;
            uiMsg.localId = netMsg.localId;
            uiMsg.chatId = netMsg.chatId;
            uiMsg.senderId = netMsg.senderId;
            uiMsg.text = decryptedText;
            uiMsg.displayTime = formatTime(netMsg.timeStamp);
            uiMsg.displayDate = formatDate(netMsg.timeStamp);

            uiMsg.showDate = false;
            return uiMsg;
        }

    };

    struct PhantomChat
    {
        qint64 tempChatId;
        QString username;
        QByteArray publicKey;
    };
}


#endif // UITYPES_H
