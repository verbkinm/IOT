#ifndef GUI_FULL_TEXT_H
#define GUI_FULL_TEXT_H

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QPlainTextEdit>

#include "device.h"

class Gui_Full_Text : public QWidget
{
    Q_OBJECT
public:
    explicit Gui_Full_Text(Device &device, QWidget *parent = nullptr);

protected:
    Device &_device;

private:
    QPlainTextEdit _text;
    QGridLayout _layout;
    QPushButton _readText, _writeText;

private slots:
    void slotRead();
    void slotWrite();


};

#endif // GUI_FULL_TEXT_H
