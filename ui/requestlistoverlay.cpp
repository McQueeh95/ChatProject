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


void RequestListOverlay::onRequestAccepted(int requestId, const QString &action)
{
    emit requestAccepted(requestId, action);
}

void RequestListOverlay::onRequestsListReceived(const QList<std::pair<int, QString>> &requests)
{
    ui->requestListView->clear();
    for(const std::pair<int, QString> &pair : requests)
    {
        QListWidgetItem* item = new QListWidgetItem(ui->requestListView);
        RequestItemWidget* widget = new RequestItemWidget(pair.first, pair.second);
        connect(widget, &RequestItemWidget::requestAction, this, &RequestListOverlay::onRequestAccepted);

        item->setSizeHint(widget->sizeHint());
        ui->requestListView->addItem(item);
        ui->requestListView->setItemWidget(item, widget);

    }
}
