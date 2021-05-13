#ifndef GUI_SERVER_H
#define GUI_SERVER_H

#include <QWidget>
#include <QMessageBox>

#include "gui_edit_server.h"

namespace Ui {
class GUI_Server;
}

class GUI_Server : public QWidget
{
    Q_OBJECT

public:
    explicit GUI_Server(const QString &name, const QString &address, const QString &imagePath,
                        QWidget *parent = nullptr);
    ~GUI_Server();

    void setDevices(uint online, uint offline);
    void setState(bool state);

private:
    Ui::GUI_Server *ui;
    QString _imagePath;

private slots:
    void slotEdit();
    void slotDelete();

signals:
    void signalConnect();
    void signalList();
    void signalEdit();
    void signalDelete();
};

#endif // GUI_SERVER_H
