#include "gui_full_text.h"

Gui_Full_Text::Gui_Full_Text(Device &device, QWidget *parent) : QWidget(parent), _device(device)
{
    _readText.setText("Read text");
    _writeText.setText("Write text");

    _layout.addWidget(&_readText, 0, 0);
    _layout.addWidget(&_writeText, 0, 1);
    _layout.addWidget(&_text, 1, 0, 1, 2);
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
    std::string text = _text.toPlainText().toStdString();
    quint16 textSize = text.size();

    Raw::RAW raw;
    raw.str = new char[textSize]; // очищается в  eraseExpectedResponseWrite
    for (quint16 i = 0; i < textSize; i++)
        raw.str[i] = text.at(i);

    _device.writeData(0, raw);
}
