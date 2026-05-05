#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QWidget>
#include "../types/networktypes.h"
#include "../models/chatviewmodel.h"
#include "../core/appcontroller.h"
#include "../models/messageviewmodel.h"
#include "../models/searchviewmodel.h"
#include <QTimer>
#include "./profilepopup.h"

namespace Ui {
class MainPage;
}

class MainPage : public QWidget
{
    Q_OBJECT

public:
    explicit MainPage(AppController* controller, QWidget *parent = nullptr);
    ~MainPage();

    void setChats(const QList<UiStruct::ChatPreview>& chats);
    void setUserId(qint64 userId);
    void setUsername(const QString& username);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::MainPage *ui;
    AppController *m_controller;
    ChatViewModel *m_chatsModel;
    MessageViewModel *m_messages;
    SearchViewModel *m_searchResults;
    QTimer *m_searchTimer;
    ProfilePopUp *m_profilePopUp;

    void showChatScreen(const QString &username);
    void hideChatScreen();

    void addNewChat(QList<UiStruct::ChatPreview> chatsList);
    void callSearch();

    void setupConnections();
    void setupViews();

    void renameObjects();

    void adjustMessageHeight();
private slots:
    //UI slots
    void onChatClicked(const QModelIndex& index);
    void onMessageSubmitted();
    void onSearchInput(const QString &text);
    void onLogout();

    //Controller slots
    void showChatHistory(const QList<UiStruct::Message>& messages);
    void addNewMessage(const UiStruct::Message &msg);
    void changeViewStatus(const UiStruct::Message &msg);
    void showSearchResult(const QList<protocol::UserSearch> &users);
    void showNoMessagesYet(const QString &username);
    void showPopUp();
    void onNetworkStateChanged(bool state);
};

#endif // MAINPAGE_H
