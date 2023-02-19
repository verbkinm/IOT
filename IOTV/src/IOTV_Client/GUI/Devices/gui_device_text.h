#ifndef GUI_DEVICE_TEXT_H
#define GUI_DEVICE_TEXT_H

#include <QPlainTextEdit>
#include <QScrollBar>

#include "gui_base_device.h"
#include "device.h"
#include "Full/gui_full_text.h"
#include "memory"

class GUI_Device_Text : public GUI_Base_Device
{
public:
    GUI_Device_Text(Device &device, QWidget *parent = nullptr);

    virtual void update() override;

private:
    QPlainTextEdit _text;
    QPushButton _editText;

private slots:
    void slotEditText();
    void slotWrite();

    std::unique_ptr<Gui_Full_Text> _full;

protected:
    virtual void slotShowFull() override;
};

#endif // GUI_DEVICE_TEXT_H
