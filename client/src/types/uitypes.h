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
                dt.toLocalTime();
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
                dt.toLocalTime();
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

    struct ChatPreview
    {
        qint64 chatId;
        qint64 peerId;
        QString peerUsername;
        QString text;
        QDateTime dt;
        QString displayDateTime;

        static QString formatDateTime(QDateTime dt)
        {
            dt = dt.toLocalTime();

            if(dt.isValid())
            {
                QDate msgDate = dt.date();
                QDate today = QDate::currentDate();

                if(msgDate == today)
                {
                    return dt.toString("HH:mm");
                }
                else if(msgDate.year() == today.year())
                {
                    return dt.toString("dd.MM");
                }
                else
                {
                    return dt.toString("dd.MM.yy");
                }
            }
            else
            {
                return "--:--:";
            }
            return "--:--:";
        }

        static ChatPreview fromNetwork(const protocol::ChatInfo &netChat, const QString &decryptedText)
        {
            ChatPreview uiChat;
            uiChat.chatId = netChat.chatId;
            uiChat.peerId = netChat.peerId;
            uiChat.peerUsername = netChat.peerUsername;
            uiChat.text = decryptedText;
            uiChat.dt = QDateTime::fromString(netChat.timeStamp, Qt::ISODate);
            uiChat.displayDateTime = formatDateTime(uiChat.dt);

            return uiChat;
        }
    };
}


#endif // UITYPES_H
