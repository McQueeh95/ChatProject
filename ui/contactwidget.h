#ifndef CONTACTWIDGET_H
#define CONTACTWIDGET_H

#include <QWidget>

namespace Ui {
class contactWidget;
}

class contactWidget : public QWidget
{
    Q_OBJECT

public:
    explicit contactWidget(QWidget *parent = nullptr);
    ~contactWidget();
    void setContactInfo(const QString& username);

private:
    Ui::contactWidget *ui;
};

#endif // CONTACTWIDGET_H
