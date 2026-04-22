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

        static Message fromNetwork(const protocol::MsgDeliv &netMsg, const QString &decryptedText)
        {
            Message uiMsg;
            uiMsg.messageId = netMsg.messageId;
            uiMsg.localId = netMsg.localId;
            uiMsg.chatId = netMsg.chatId;
            uiMsg.senderId = netMsg.senderId;
            uiMsg.text = decryptedText;
            uiMsg.displayTime = formatTime(netMsg.timeStamp);
            return uiMsg;
        }

    };
}


#endif // UITYPES_H
