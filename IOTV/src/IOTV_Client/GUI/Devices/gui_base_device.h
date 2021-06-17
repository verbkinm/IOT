#ifndef GUI_BASE_DEVICE_H
#define GUI_BASE_DEVICE_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>

#include "Patterns/observer.h"

#include "memory"

class Device;

namespace Ui {
class GUI_Base_Device;
}

class GUI_Base_Device : public QFrame, public Observer
{
    Q_OBJECT

public:
    GUI_Base_Device(Device &device, QWidget *parent = nullptr);
    ~GUI_Base_Device();

protected:
    Ui::GUI_Base_Device *ui;

    Device &_device;

    QGridLayout _main_layout;

    QLabel _viewName;
    QPushButton _info, _settingsDevice;

    void setViewNameFont();

private slots:
    void slotInfoPresses();
};

#endif // GUI_BASE_DEVICE_H
