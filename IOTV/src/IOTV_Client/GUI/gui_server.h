#ifndef GUI_SERVER_H
#define GUI_SERVER_H

#include <QFrame>
#include <QMessageBox>
#include <QAbstractSocket>

#include "gui_edit_server.h"
#include "Patterns/observer.h"
#include "tab.h"
#include "device.h"

class Server;

namespace Ui {
class GUI_Server;
}

class GUI_Server : public QFrame, public Observer
{
    Q_OBJECT

public:
    GUI_Server(Server &server, const Tab &serverTab, QWidget *parent = nullptr);

    ~GUI_Server();

    const std::map<QString, std::shared_ptr<Device> > &getDevices() const;
    void setDevicesState(uint online, uint offline);
    void setState(QAbstractSocket::SocketState state);
     QAbstractSocket::SocketState state() const;

    virtual void update() override;

private:
    Ui::GUI_Server *ui;
    QString _imagePath;

    Server &_server;
    const Tab &_serverTab;

private slots:
    void slotConnect();
    void slotList();
    void slotEdit();
};

#endif // GUI_SERVER_H
