#ifndef GUI_BASE_DEVICE_H
#define GUI_BASE_DEVICE_H

#include <QWidget>

#include "device.h"

namespace Ui {
class GUI_Base_Device;
}

class GUI_Base_Device : public QWidget
{
    Q_OBJECT

public:
    explicit GUI_Base_Device(Device &device, QWidget *parent = nullptr);
    ~GUI_Base_Device();

    virtual void update() = 0;

protected:
    Ui::GUI_Base_Device *ui;

    uint8_t _id;
    QString _description;

    Device &_device;

};

#endif // GUI_BASE_DEVICE_H
