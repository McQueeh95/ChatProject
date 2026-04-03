#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QWidget>
#include "../networktypes.h"
#include "../models/contactlistmodel.h"
#include "../appcontroller.h"
#include "../models/messageviewmodel.h"
#include "../models/searchviewmodel.h"
#include <QTimer>

namespace Ui {
class MainPage;
}

class MainPage : public QWidget
{
    Q_OBJECT

public:
    explicit MainPage(AppController* controller, QWidget *parent = nullptr);
    ~MainPage();
    void setChats(const QList<protocol::ChatInfo>& chats);
    void setUserId(qint64 userId);

private:
    Ui::MainPage *ui;
    AppController *m_controller;
    ContactListModel *m_chatsModel;
    MessageViewModel *m_messages;
    SearchViewModel *m_searchResults;
    QTimer *m_searchTimer;
    void showChatScreen(const QString &username);
    void hideChatScreen();
    void addNewChat(const protocol::ChatInfo &chat);
    void callSearch();



private slots:
    void onChatClicked(const QModelIndex& index);
    void sendMessage();
    void showChatHistory(qint64 chatId, const QList<protocol::MsgDeliv>& messages);
    void addNewMessage(const protocol::MsgDeliv &msg);
    void ChangeViewStatus(const protocol::MsgDeliv &msg);
    void onSearchInput(const QString &text);
    void showSearchResult(const QList<protocol::UserSearch> &users);

};

#endif // MAINPAGE_H
