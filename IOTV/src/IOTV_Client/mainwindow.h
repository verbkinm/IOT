#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"
#include "GUI/tab.h"


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
    void on_actionAdd_room_triggered();

    void on_actionDelete_triggered();

    void slotAddDevice();
    void slotTabChange(int);

    void on_actionAdd_server_triggered();
    void on_actionRemove_server_triggered();

private:
    Ui::MainWindow *ui;

    std::list<Server*> _servers;
    std::list<GUI_Server*> _gui_servers;

    Tab _serverTab;

};
#endif // MAINWINDOW_H
