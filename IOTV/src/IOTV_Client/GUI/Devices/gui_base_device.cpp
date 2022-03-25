#include "gui_base_device.h"
#include "device.h"

GUI_Base_Device::GUI_Base_Device(Device &device, QWidget *parent) :
    QFrame(parent), Observer(), _device(device)
{
    setFrameStyle(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    device.attach(this);

    _viewName.setText(_device.getViewName());
    setViewNameFont();

    _info.setFixedSize(28, 28);
    _info.setIcon(QIcon(":/devices/info"));
    _info.setIconSize(QSize(26, 26));

    _settingsDevice.setFixedSize(28, 28);
    _settingsDevice.setIcon(QIcon(":/edit"));
    _settingsDevice.setIconSize(QSize(26, 26));

    _showFull.setFixedSize(28, 28);
    _showFull.setIcon(QIcon(":/devices/showFull"));
    _showFull.setIconSize(QSize(26, 26));

    _buttonLayout.addStretch();
    _buttonLayout.addWidget(&_settingsDevice);
    _buttonLayout.addWidget(&_info);
    _buttonLayout.addWidget(&_showFull);

    _main_layout.addWidget(&_viewName, 0, 0, 1, 3, Qt::AlignCenter);
    _main_layout.addLayout(&_buttonLayout, 1, 0, 1, 3, Qt::AlignCenter);

    setLayout(&_main_layout);
    setWindowTitle(_device.getName() + " (" + QString::number(_device.getId()) + ")");

    connect(&_info, &QPushButton::clicked, this, &GUI_Base_Device::slotInfoPresses);
    connect(&_settingsDevice, &QPushButton::clicked, this, &GUI_Base_Device::slotSettingPressed);
    connect(&_showFull, &QPushButton::clicked, this, &GUI_Base_Device::slotShowFull);

    setMaximumSize(200, 150);

    newObjectName();
}

GUI_Base_Device::~GUI_Base_Device()
{
    _device.setAutoReadEnable(false);
    _device.detach(this);
}

QString GUI_Base_Device::getServerObjectName() const
{
    return _device.getServerObjectName();
}

QString GUI_Base_Device::getName() const
{
    return _device.getName();
}

void GUI_Base_Device::setViewName(const QString &name)
{
    _viewName.setText(name);
}

void GUI_Base_Device::setViewNameFont()
{
    QFont font = _viewName.font();
    font.setPixelSize(18);
    font.setBold(true);
    _viewName.setFont(font);
}

void GUI_Base_Device::stateAndViewName()
{
    if(_device.isOnline() && !isEnabled())
        setEnabled(true);
    else if(!_device.isOnline() && isEnabled())
    {
        setEnabled(false);
    }

    if(_device.getViewName() != _viewName.text())
        setViewName(_device.getViewName());
}

void GUI_Base_Device::newObjectName()
{
    this->setObjectName(_device.objectName());
}

void GUI_Base_Device::slotInfoPresses()
{
    QString state;
    if(_device.isOnline())
        state = "Connected.";
    else
        state = "Not connect.";

    QString info = "Device id: " + QString::number(_device.getId())
            + "\nDevice name: " + _device.getName()
            + "\nDescription: " + _device.getDescription()
            + "\nR/W: " + QString::number(_device.readChannelLength()) + "/" + QString::number(_device.writeChannelLength())
            + "\nServer: " + _device.getServerObjectName();
    QMessageBox::information(this, "Device description", info, QMessageBox::Ok);
}

void GUI_Base_Device::slotSettingPressed()
{
    GUI_edit_base_device gui_edit_device(_device.getViewName());
    gui_edit_device.setWindowTitle(_device.getName());
    gui_edit_device.setWindowIcon(QIcon(":/server"));

    if(gui_edit_device.exec() == QDialog::Accepted)
    {
        if(gui_edit_device.viewName().length())
        {
            _device.setViewName(gui_edit_device.viewName());
            emit signalSettingsEdited();
        }
    }
}

