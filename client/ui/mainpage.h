#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QWidget>
#include "../networktypes.h"
#include "../models/contactlistmodel.h"
#include "../appcontroller.h"
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

    void setChats(const QList<protocol::ChatInfo>& chats);
    void setUserId(qint64 userId);
    void setUsername(const QString& username);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::MainPage *ui;
    AppController *m_controller;
    ContactListModel *m_chatsModel;
    MessageViewModel *m_messages;
    SearchViewModel *m_searchResults;
    QTimer *m_searchTimer;
    ProfilePopUp *m_profilePopUp;

    void showChatScreen(const QString &username);
    void hideChatScreen();

    void addNewChat(const protocol::ChatInfo &chat);
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

    //Controller slots
    void showChatHistory(qint64 chatId, const QList<UiStruct::Message>& messages);
    void addNewMessage(const UiStruct::Message &msg);
    void changeViewStatus(const UiStruct::Message &msg);
    void showSearchResult(const QList<protocol::UserSearch> &users);
    void showNoMessagesYet(const QString &username);
    void showPopUp();
};

#endif // MAINPAGE_H
