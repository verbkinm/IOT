#include "gui_full_text.h"

Gui_Full_Text::Gui_Full_Text(Device &device, QWidget *parent) : QWidget(parent), _device(device)
{
    _readText.setText("Read text");
    _writeText.setText("Write text");

    _layout.addWidget(&_readText);
    _layout.addWidget(&_writeText);
    _layout.addWidget(&_text);
    setLayout(&_layout);

    slotRead();

    connect(&_readText, &QPushButton::clicked, this, &Gui_Full_Text::slotRead);
    connect(&_writeText, &QPushButton::clicked, this, &Gui_Full_Text::slotWrite);
}

void Gui_Full_Text::slotRead()
{
    _text.clear();
    _text.setPlainText(_device.getReadChannelData(0).str);
}

void Gui_Full_Text::slotWrite()
{
    Raw::RAW raw = _device.getReadChannelData(0);
    delete[] raw.str;

    QString text = _text.toPlainText();
    uint16_t textSize = text.size();

    char *ptr = new char[text.size() + 1];
    for (uint16_t i = 0; i < textSize; i++)
        ptr[i] = text.at(i).toLatin1();
    ptr[textSize] = '\0';

    raw.str = ptr;
    _device.setReadChannelData(0, raw);

    _device.writeData(0, raw);
}
