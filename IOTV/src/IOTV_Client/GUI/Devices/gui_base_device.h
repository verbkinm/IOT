#ifndef GUI_BASE_DEVICE_H
#define GUI_BASE_DEVICE_H

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>

#include "Patterns/observer.h"
#include "gui_edit_base_device.h"

class Device;

class GUI_Base_Device : public QFrame, public Observer
{
    Q_OBJECT

public:
    GUI_Base_Device(Device &device, QWidget *parent = nullptr);
    ~GUI_Base_Device();

    QString getServerObjectName() const;
    QString getName() const;

protected:
    void setViewName(const QString &name);
    void stateAndViewName();

    Device &_device;

    QGridLayout _main_layout;
    QLabel _viewName;
    QPushButton _info, _settingsDevice, _showFull;

private:
    void newObjectName();
    void setViewNameFont();

    QHBoxLayout _buttonLayout;

private slots:
    void slotInfoPresses();
    void slotSettingPressed();

protected slots:
    virtual void slotShowFull(){};

signals:
    void signalSettingsEdited();
};

#endif // GUI_BASE_DEVICE_H
