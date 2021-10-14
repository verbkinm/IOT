#include "gui_edit_server.h"
#include "ui_gui_edit_server.h"

GUI_Edit_Server::GUI_Edit_Server(const QString &name,
                                 const QString &address, const QString &port,
                                 const QString &imagePath,
                                 QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gui_edit_server)
{
    ui->setupUi(this);

    ui->name->setText(name);
    ui->address->setText(address);
    ui->port->setText(port);
    ui->imagePath->setText(imagePath);

    connect(ui->imagePathButton, &QAbstractButton::clicked, this, &GUI_Edit_Server::slotSelectImage);
}

GUI_Edit_Server::~GUI_Edit_Server()
{
    delete ui;
}

QString GUI_Edit_Server::getName() const
{
    return ui->name->text();
}

QString GUI_Edit_Server::getAddress() const
{
    return ui->address->text();
}

QString GUI_Edit_Server::getPort() const
{
    return ui->port->text();
}

QString GUI_Edit_Server::getImagePath() const
{
    return ui->imagePath->text();
}

void GUI_Edit_Server::slotSelectImage()
{
    QString result = QFileDialog::getOpenFileName(this, "Select image", ui->imagePath->text(), "*.png *.jpg *.ico *.bmp");
    if(!result.isEmpty())
        ui->imagePath->setText(result);
}
