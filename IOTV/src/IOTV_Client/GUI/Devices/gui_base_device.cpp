#include "gui_base_device.h"
#include "ui_gui_base_device.h"
#include "device.h"


GUI_Base_Device::GUI_Base_Device(std::shared_ptr<Device> device, QWidget *parent) :
    QWidget(parent), Observer(),
    ui(new Ui::GUI_Base_Device), _device(device)
{
    device->attach(this);
    ui->setupUi(this);    

    _viewName.setText(_device->getName());
    setViewNameFont();

    _info.setFixedSize(48, 48);
    _info.setIcon(QIcon(":/devices/info"));
    _info.setIconSize(QSize(36, 36));

    _main_layout.addWidget(&_viewName, 0, 0, Qt::AlignLeft);
    _main_layout.addWidget(&_info, 0, 1, Qt::AlignRight);

    setLayout(&_main_layout);
    resize(280, 180);
    setWindowTitle(_device->getName() + " (" + QString::number(_device->getId()) + ")");

    connect(&_info, &QPushButton::clicked, this, &GUI_Base_Device::slotInfoPresses);
}

GUI_Base_Device::~GUI_Base_Device()
{
    delete ui;
}

void GUI_Base_Device::setViewNameFont()
{
    QFont font = _viewName.font();
    font.setPixelSize(24);
    font.setBold(true);
    _viewName.setFont(font);
}

void GUI_Base_Device::slotInfoPresses()
{
    QMessageBox::information(this, "Device description", _device->getDescription(), QMessageBox::Ok);
}
