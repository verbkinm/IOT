#ifndef GUI_BASE_DEVICE_H
#define GUI_BASE_DEVICE_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

#include "Patterns/observer.h"

#include "memory"

class Device;

namespace Ui {
class GUI_Base_Device;
}

class GUI_Base_Device : public QWidget, public Observer
{
    Q_OBJECT

public:
    GUI_Base_Device(std::shared_ptr<Device> device, QWidget *parent = nullptr);
    ~GUI_Base_Device();

    virtual void update() override{};

protected:
    Ui::GUI_Base_Device *ui;

    std::shared_ptr<Device> _device;

    QGridLayout _main_layout;

    QLabel _viewName;
    QPushButton _info;

    void setViewNameFont();

private slots:
    void slotInfoPresses();
};

#endif // GUI_BASE_DEVICE_H
