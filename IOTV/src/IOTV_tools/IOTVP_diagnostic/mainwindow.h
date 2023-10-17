#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QGridLayout>
#include <QLabel>

#include "raw.h"
#include "creatorpkgs.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void printHeaderData(const Header *header);
    void printIdentificationData(const Identification *body);
    void printReadWriteData(const Read_Write *body);
    void printStateData(const State *body);
    void printTechData(const Tech *body);


    QString headerType(uint8_t type) const;
    QString headerAssignment(uint8_t assignment) const;
    QString staetType(uint8_t state) const;
    QString techType(uint8_t type) const;

private slots:
    void on_action_triggered();

    void slotConvert();

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
