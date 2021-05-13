#include "gui_base_device.h"
#include "ui_gui_base_device.h"

GUI_Base_Device::GUI_Base_Device(Device &device, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GUI_Base_Device), _id(0), _description(""), _device(device)
{
    ui->setupUi(this);    
}

GUI_Base_Device::~GUI_Base_Device()
{
    delete ui;
}
