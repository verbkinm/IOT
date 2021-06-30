#include "gui_edit_base_device.h"
#include "ui_gui_edit_base_device.h"

GUI_edit_base_device::GUI_edit_base_device(const QString &viewName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GUI_edit_base_device)
{
    ui->setupUi(this);

    ui->viewName->setText(viewName);
}

GUI_edit_base_device::~GUI_edit_base_device()
{
    delete ui;
}

QString GUI_edit_base_device::viewName() const
{
    return ui->viewName->text();
}
