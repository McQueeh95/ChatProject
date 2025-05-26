#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>

namespace Ui {
class RegisterWidget;
}

class RegisterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWidget(QWidget *parent = nullptr);
    ~RegisterWidget();

private:
    Ui::RegisterWidget *ui;
signals:
    void goToMainW(const QString& username);
    void usernameInputted(const QString& username);
public slots:
    void onUsernameButtonClicked();
};

#endif // REGISTERWIDGET_H
