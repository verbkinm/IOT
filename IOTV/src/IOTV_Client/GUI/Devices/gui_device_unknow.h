#ifndef GUI_DEVICE_UNKNOW_H
#define GUI_DEVICE_UNKNOW_H

#include "gui_base_device.h"

class GUI_Device_Unknow : public GUI_Base_Device
{
public:
    GUI_Device_Unknow(Device &device, QWidget *parent = nullptr);

    virtual void update() override;

private:
    QLabel _label;
};

#endif // GUI_DEVICE_UNKNOW_H
