#include "devicelist.h"
#include "ui_devicelist.h"

DeviceList::DeviceList(const std::map<QString, std::unique_ptr<Device> > &devices, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceList),
    _devices(devices)
{
    ui->setupUi(this);

    ui->scrollAreaWidgetContents->installEventFilter(this);
}

DeviceList::~DeviceList()
{
    delete ui;
}

void DeviceList::list()
{
    for (auto &elem : _devices)
    {
        int count = ui->layout->count();
        int row = count / 3;
        int column = count % 3;

        GUI_Base_Device* d = GUI_Device_Creator::createGUIDevice(*elem.second);
        d->setEnabled(false);
        ui->layout->addWidget(d, row, column);
    }
}

void DeviceList::layoutClear()
{
    QLayoutItem *item;
    while( (item = ui->layout->itemAt(0)) )
    {
        ui->layout->removeItem( item );
        ui->layout->removeWidget(item->widget());
        delete item;
    }

    while(ui->scrollAreaWidgetContents->children().length() != 1)
    {
        QObject *object = ui->scrollAreaWidgetContents->children().at(1);
        if(object->objectName() == "GUI_Base_Device")
            delete object;
    }
}

bool DeviceList::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)

    if(event->type() == QEvent::ChildRemoved && this->isVisible() && _devices.size())
    {
        ui->scrollAreaWidgetContents->removeEventFilter(this);
        layoutClear();
        list();
        ui->scrollAreaWidgetContents->installEventFilter(this);

        return false;
    }

    return true;
}

void DeviceList::closeEvent(QCloseEvent *event)
{
    ui->scrollAreaWidgetContents->removeEventFilter(this);
    layoutClear();

    event->accept();
}

