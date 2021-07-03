#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QInputDialog>
#include <QSettings>

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
    void slotTabChange(int index);
    void slotCloseTab(int index);
    void slotRenameTab(int index);

    void on_actionAdd_room_triggered();
    void on_actionDelete_room_triggered();

    void on_actionAdd_server_triggered();
    void on_actionRemove_server_triggered();

    void slotRoomsRestructWidget();

private:
    void checkSettingsFileExist();
    void readSettings();
    void saveSettingsServers();
    void saveSettingsRooms();

    Ui::MainWindow *ui;

    Tab _serverTab;
    std::list<Server*> _serverList;
    QSettings _settings;

protected:
    virtual void closeEvent(QCloseEvent *) override;
};
#endif // MAINWINDOW_H
