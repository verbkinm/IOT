#ifndef GUI_DEVICE_SWITCH_H
#define GUI_DEVICE_SWITCH_H

#include "gui_base_device.h"
#include "device.h"

class GUI_Device_Switch : public GUI_Base_Device
{
public:
    GUI_Device_Switch(Device &device, QWidget *parent = nullptr);

    virtual void update() override;

private:
    QPushButton _button;
    bool _buttonState;

private slots:
    void slotButtonPressed();
};

#endif // GUI_DEVICE_SWITCH_H
