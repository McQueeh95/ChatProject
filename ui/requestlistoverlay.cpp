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

void RequestListOverlay::onRequestsListReceived(const QList<QString> &requests)
{
    for(const QString &name : requests)
    {
        QListWidgetItem* item = new QListWidgetItem(ui->requestListView);
        RequestItemWidget* widget = new RequestItemWidget(name);

        item->setSizeHint(widget->sizeHint());
        ui->requestListView->addItem(item);
        ui->requestListView->setItemWidget(item, widget);
    }
}
