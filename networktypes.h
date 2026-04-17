#ifndef NETWORKTYPES_H
#define NETWORKTYPES_H
#include <QString>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

namespace protocol
{
    enum class messageType
    {
        LOGIN = 0,
        FORW = 1,
        REG = 2,
        LOGIN_RES = 3,
        REG_RES = 4,
        SEARCH_REQ = 5,
        SEARCH_RES = 6,
        LOGOUT = 7,
        DELIV_ACK = 8,
        READ_ACK = 9,
        HISTORY_REQ = 10,
        HISTORY_RES = 11,
        START_CHAT_REQ = 12,
        NEW_CHAT_EVENT = 13
    };

    struct ChatInfo{
        qint64 chatId;
        qint64 peerId;
        QString peerUsername;

        static ChatInfo fromJson(const QJsonObject& json)
        {
            ChatInfo chat;
            chat.chatId = json["chat_id"].toVariant().toLongLong();
            chat.peerId = json["peer_id"].toVariant().toLongLong();
            chat.peerUsername = json["peer_username"].toVariant().toString();
            return chat;
        }
    };

    struct ForwardReq
    {
        qint64 localId;
        qint64 chatId;
        QString payload;

        QJsonObject toJson() const
        {
            QJsonObject json;
            json["type"] = static_cast<qint8>(messageType::FORW);
            json["local_id"] = localId;
            json["chat_id"] = chatId;
            json["payload"] = payload;
            return json;
        }
    };

    struct StartChatReq
    {
        qint64 msgLocalId;
        qint64 targetId;
        QString payload;

        QJsonObject toJson() const
        {
            QJsonObject json;
            json["type"] = static_cast<qint8>(messageType::START_CHAT_REQ);
            json["local_id"] = msgLocalId;
            json["target_id"] = targetId;
            json["payload"] = payload;
            return json;
        }
    };

    struct MsgDeliv
    {
        qint64 messageId;
        qint64 localId;
        qint64 chatId;
        qint64 senderId;
        QString payload;
        QString timeStamp;
        QString displayTime;

        static MsgDeliv fromJson(const QJsonObject& json)
        {
            MsgDeliv msg;
            msg.messageId = json["message_id"].toVariant().toLongLong();
            msg.localId = json["local_id"].toVariant().toLongLong();
            msg.chatId = json["chat_id"].toVariant().toLongLong();
            msg.senderId = json["sender_id"].toVariant().toLongLong();
            msg.payload = json["payload"].toVariant().toString();
            msg.timeStamp = json["timestamp"].toVariant().toString();
            QDateTime dt = QDateTime::fromString(msg.timeStamp, Qt::ISODate);
            if(dt.isValid())
            {
                msg.displayTime = dt.toString("HH:mm");
            }
            else
            {
                qDebug() << "dt is not valid";
                msg.displayTime = msg.timeStamp;
            }
            return msg;
        }
    };

    struct LoginReq
    {
        QString username;
        QString hashedPassword;
        QJsonObject toJson() const
        {
            QJsonObject json;
            json["type"] = static_cast<qint8>(messageType::LOGIN);
            json["username"] = username;
            json["hashed_password"] = hashedPassword;
            return json;
        }
    };

    struct RegisterReq
    {
        QString username;
        QByteArray authKey;
        QByteArray salt;
        QByteArray publicKey;
        QByteArray encryptedVault;
        QByteArray vaultNonce;

        QJsonObject toJson() const
        {
            QJsonObject json;
            json["type"] = static_cast<qint8>(messageType::REG);
            json["username"] = username;
            json["auth_key"] = QString::fromLatin1(authKey.toBase64());
            json["salt"] = QString::fromLatin1(salt.toBase64());
            json["public_key"] = QString::fromLatin1(publicKey.toBase64());
            json["encrypted_vault"] = QString::fromLatin1(encryptedVault.toBase64());
            json["vault_nonce"] = QString::fromLatin1(vaultNonce.toBase64());
            return json;
        }
    };

    struct LoginRes
    {
        QString status;
        qint64 userId;
        QString errorMsg;
        QHash<qint64, ChatInfo> chats;

        static LoginRes fromJson(const QJsonObject& json)
        {
            LoginRes msg;
            msg.status = json["status"].toVariant().toString();
            if(msg.status == "ok")
            {
                msg.userId = json["user_id"].toVariant().toLongLong();
                QJsonArray chatsArray = json["chats"].toArray();
                msg.chats.reserve(chatsArray.size());
                for(const QJsonValue& val: chatsArray)
                {
                    ChatInfo chat = ChatInfo::fromJson(val.toObject());

                    msg.chats.insert(chat.chatId, chat);
                }
            }
            else
                msg.errorMsg = json["error_msg"].toVariant().toString();

            return msg;
        }
    };

    struct RegRes
    {
        QString status;
        qint64 userId;
        QString errorMsg;

        static RegRes fromJson(const QJsonObject& json)
        {
            RegRes msg;
            msg.status = json["status"].toVariant().toString();
            if(msg.status == "ok")
            {
                msg.userId = json["user_id"].toVariant().toLongLong();
            }
            else
                msg.errorMsg = json["error_msg"].toVariant().toString();
            return msg;
        }
    };

    struct SearchReq
    {
        QString toFind;

        QJsonObject toJson() const
        {
            QJsonObject json;
            json["type"] = static_cast<qint8>(messageType::SEARCH_REQ);
            json["to_find"] = toFind;
            return json;
        }
    };

    struct UserSearch
    {
        qint64 userId;
        QString username;

        static UserSearch fromJson(const QJsonObject& json)
        {
            UserSearch user;
            user.userId = json["user_id"].toVariant().toLongLong();
            user.username = json["username"].toVariant().toString();
            return user;
        }
    };

    struct SearchRes
    {
        QList<UserSearch> foundUsers;

        static SearchRes fromJson(const QJsonObject& json)
        {
            SearchRes searchRes;
            QJsonArray usersArray = json["found_users"].toArray();
            searchRes.foundUsers.reserve(usersArray.size());
            for(const QJsonValue& val: usersArray)
            {
                UserSearch chat = UserSearch::fromJson(val.toObject());

                searchRes.foundUsers.push_back(chat);
            }
            return searchRes;
        }
    };

    struct HistoryReq
    {
        qint64 chatId;

        QJsonObject toJson() const
        {
            QJsonObject json;
            json["type"] = static_cast<qint8>(messageType::HISTORY_REQ);
            json["chat_id"] = chatId;
            return json;
        }
    };

    struct HistoryRes
    {
        QString status;
        QString errorMessage;
        qint64 chatId;
        QList<MsgDeliv> messages;

        static HistoryRes fromJson(const QJsonObject& json)
        {
            HistoryRes msg;
            msg.status = json["status"].toVariant().toString();
            msg.chatId = json["chat_id"].toVariant().toLongLong();
            if(msg.status == "ok")
            {
                QJsonArray messagesArray = json["messages"].toArray();
                msg.messages.reserve(messagesArray.size());
                for(const QJsonValue& val : messagesArray)
                {
                    MsgDeliv message = MsgDeliv::fromJson(val.toObject());

                    msg.messages.push_back(message);
                }
            }
            else
            {
                msg.errorMessage = json["error_msg"].toVariant().toString();
            }
            return msg;
        }
    };

    struct DelivAck
    {
        QString status;
        qint64 chatId = - 1;
        qint64 localId = -1;
        qint64 realId = -1;
        QString timestamp;
        QString displayTime;
        QString errorMsg;
        qint64 peerId = -1;

        static DelivAck fromJson(const QJsonObject& json)
        {
            DelivAck msg;
            msg.status = json["status"].toVariant().toString();
            msg.localId = json["local_id"].toVariant().toLongLong();
            msg.chatId = json["chat_id"].toVariant().toLongLong();
            if(msg.status == "ok")
            {
                msg.realId = json["real_id"].toVariant().toLongLong();
                msg.timestamp = json["timestamp"].toVariant().toString();
                QDateTime dt = QDateTime::fromString(msg.timestamp, Qt::ISODate);
                qint64 peerId = json["peer_id"].toVariant().toLongLong();
                if(peerId > 0)
                    msg.peerId = peerId;
                else
                    msg.peerId = -1;

                if(dt.isValid())
                    msg.displayTime = dt.toString("HH:mm");
                else
                    msg.displayTime = msg.timestamp;
            }
            else
            {
                msg.errorMsg = json["error_msg"].toVariant().toString();
            }
            return msg;
        }
    };
}


#endif // NETWORKTYPES_H
