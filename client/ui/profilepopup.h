#ifndef PROFILEPOPUP_H
#define PROFILEPOPUP_H

#include <QFrame>

namespace Ui {
class ProfilePopUp;
}

class ProfilePopUp : public QFrame
{
    Q_OBJECT

public:
    explicit ProfilePopUp(QWidget *parent = nullptr);
    ~ProfilePopUp();
    void setUsername(const QString& name);

private:
    Ui::ProfilePopUp *ui;

private slots:
    void logoutRequsted();

signals:
    void logout();
};

#endif // PROFILEPOPUP_H
