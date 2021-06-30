#ifndef GUI_BASE_DEVICE_H
#define GUI_BASE_DEVICE_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>

#include "Patterns/observer.h"
#include "gui_edit_base_device.h"

#include "memory"

class Device;

class GUI_Base_Device : public QFrame, public Observer
{
    Q_OBJECT

public:
    GUI_Base_Device(Device &device, QWidget *parent = nullptr);
    ~GUI_Base_Device();

protected:
    void setViewName(const QString &name);

    Device &_device;

    QGridLayout _main_layout;
    QHBoxLayout _buttonLayout;

    QLabel _viewName;
    QPushButton _info, _settingsDevice;

private:
    void newObjectName();
    void setViewNameFont();

private slots:
    void slotInfoPresses();
    void slotSettingPressed();
};

#endif // GUI_BASE_DEVICE_H
