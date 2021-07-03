#include "gui_device_cap.h"

GUI_Device_Cap::GUI_Device_Cap(const QString &server, const QString &name, QWidget *parent) : QFrame(parent)
{
    setFrameStyle(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);

    _name.setText(name);
    QFont font = _name.font();
    font.setPixelSize(18);
    font.setBold(true);
    _name.setFont(font);

    _server.setText(server);
    _label.setPixmap(QPixmap(":/devices/unknow"));

    _layout.addWidget(&_name, 0, 0, Qt::AlignCenter);
    _layout.addWidget(&_server, 1, 0, Qt::AlignCenter);
    _layout.addWidget(&_label, 2, 0, Qt::AlignCenter);

    setLayout(&_layout);
    setMaximumSize(200, 150);
}

QString GUI_Device_Cap::getServerName() const
{
    return _server.text();
}

QString GUI_Device_Cap::getName() const
{
    return _name.text();
}
