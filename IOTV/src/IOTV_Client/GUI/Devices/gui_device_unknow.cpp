#include "gui_device_unknow.h"
#include "device.h"

GUI_Device_Unknow::GUI_Device_Unknow(Device &device, QWidget *parent) : GUI_Base_Device(device, parent)
{
    _label.setPixmap(QPixmap(":/devices/unknow"));

    _main_layout.addWidget(&_label, 2, 0, 1, 3, Qt::AlignCenter);
}

void GUI_Device_Unknow::update()
{
    setWindowTitle(QString::number(_device.getId()));
    _viewName.setText(_device.getName());

    if(_device.getState())
        setEnabled(true);
    else
        setEnabled(false);

    setViewName(_device.getViewName());
}

