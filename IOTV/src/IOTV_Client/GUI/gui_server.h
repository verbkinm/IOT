#ifndef GUI_SERVER_H
#define GUI_SERVER_H

#include <QFrame>
#include <QMessageBox>
#include <QAbstractSocket>

#include "gui_edit_server.h"
#include "Patterns/observer.h"

class Server;

namespace Ui {
class GUI_Server;
}

class GUI_Server : public QFrame, public Observer
{
    Q_OBJECT

public:
    explicit GUI_Server(Server &server,
                        const QString &imagePath = ":/server",
                        QWidget *parent = nullptr);

    ~GUI_Server();

    void setDevices(uint online, uint offline);
    void setState(QAbstractSocket::SocketState state);

    virtual void update() override;

private:
    Ui::GUI_Server *ui;
    QString _imagePath;

    Server &_server;

private slots:
    void slotEdit();
    void slotDelete();

    void slotConnect();
    void slotList();

signals:
    void signalEdit();
    void signalDelete();

};

#endif // GUI_SERVER_H
