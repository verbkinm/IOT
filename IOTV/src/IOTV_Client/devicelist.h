#ifndef DEVICELIST_H
#define DEVICELIST_H

#include <QWidget>
#include <QCloseEvent>

#include <GUI/Devices/gui_device_creator.h>

namespace Ui {
class DeviceList;
}

class DeviceList : public QWidget
{
    Q_OBJECT

public:
    DeviceList(const std::map<QString, std::unique_ptr<Device> > &devices, QWidget *parent = nullptr);
    ~DeviceList();

    void list();

private:
    Ui::DeviceList *ui;

    const std::map<QString, std::unique_ptr<Device>> &_devices;

    void layoutClear();

public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;
};

#endif // DEVICELIST_H
