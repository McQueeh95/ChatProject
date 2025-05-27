#ifndef ADDCONTACTPOPUP_H
#define ADDCONTACTPOPUP_H

#include <QDialog>

namespace Ui {
class AddContactPopup;
}

class AddContactPopup : public QDialog
{
    Q_OBJECT

public:
    explicit AddContactPopup(QWidget *parent = nullptr);
    ~AddContactPopup();

private:
    Ui::AddContactPopup *ui;
private slots:
    void onOkButtonClicked();
signals:
    void uuidToSend(const QString& contactUuid);
};

#endif // ADDCONTACTPOPUP_H
