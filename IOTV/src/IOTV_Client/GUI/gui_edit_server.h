#ifndef GUI_EDIT_SERVER_H
#define GUI_EDIT_SERVER_H

#include <QFileDialog>

namespace Ui {
class gui_edit_server;
}

class GUI_Edit_Server : public QDialog
{
    Q_OBJECT

public:
    explicit GUI_Edit_Server(const QString &name, const QString &address, const QString &port, const QString &imagePath,
                             QWidget *parent = nullptr);
    ~GUI_Edit_Server();

    QString getName() const;
    QString getAddress() const;
    QString getPort() const;
    QString getImagePath() const;

private:
    Ui::gui_edit_server *ui;

private slots:
    void slotSelectImage();
};

#endif // GUI_EDIT_SERVER_H
