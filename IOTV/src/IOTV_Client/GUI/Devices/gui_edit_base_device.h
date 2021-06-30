#ifndef GUI_EDIT_BASE_DEVICE_H
#define GUI_EDIT_BASE_DEVICE_H

#include <QDialog>

namespace Ui {
class GUI_edit_base_device;
}

class GUI_edit_base_device : public QDialog
{
    Q_OBJECT

public:
    GUI_edit_base_device(const QString &viewName, QWidget *parent = nullptr);
    ~GUI_edit_base_device();

    QString viewName() const;

private:
    Ui::GUI_edit_base_device *ui;
};

#endif // GUI_EDIT_BASE_DEVICE_H
