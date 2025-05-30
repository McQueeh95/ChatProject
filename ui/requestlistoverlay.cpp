#include "requestlistoverlay.h"
#include "ui_requestlistoverlay.h"
#include "requestitemwidget.h"

RequestListOverlay::RequestListOverlay(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RequestListOverlay)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    ui->setupUi(this);
}

RequestListOverlay::~RequestListOverlay()
{
    delete ui;
}


void RequestListOverlay::onRequestActionReceived(int contactId, const QString &action)
{
    emit requestActionReceived(contactId, action);
}

void RequestListOverlay::onRequestsListReceived(const QList<std::pair<int, QString>> &requests)
{
    ui->requestListView->clear();
    for(const std::pair<int, QString> &pair : requests)
    {
        QListWidgetItem* item = new QListWidgetItem(ui->requestListView);
        RequestItemWidget* widget = new RequestItemWidget(pair.first, pair.second);
        connect(widget, &RequestItemWidget::requestAction, this, &RequestListOverlay::onRequestActionReceived);

        item->setSizeHint(widget->sizeHint());
        ui->requestListView->addItem(item);
        ui->requestListView->setItemWidget(item, widget);

    }
}
