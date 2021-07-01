#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>

#include "server.h"
#include "GUI/tab_room.h"
#include "GUI/objectlist.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void slotAddDeviceToRoom();
    void slotRemoveDeviceFromRoom();
    void slotTabChange(int);

    void on_actionAdd_room_triggered();
    void on_actionDelete_triggered();

    void on_actionAdd_server_triggered();
    void on_actionRemove_server_triggered();

private:
    void roomsRestructWidget();

    Ui::MainWindow *ui;

    Tab _serverTab;
    std::list<Server*> _serverList;

protected:
    virtual void closeEvent(QCloseEvent *) override;
};
#endif // MAINWINDOW_H
