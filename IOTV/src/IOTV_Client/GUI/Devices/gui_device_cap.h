#ifndef GUI_DEVICE_CAP_H
#define GUI_DEVICE_CAP_H

#include <QFrame>
#include <QLabel>
#include <QGridLayout>

class GUI_Device_Cap : public QFrame
{
    Q_OBJECT
public:
    GUI_Device_Cap(const QString &server, const QString &name, QWidget *parent = nullptr);

    QString getServerName() const;
    QString getName() const;

private:
    QLabel _name, _server, _label;
    QGridLayout _layout;
};

#endif // GUI_DEVICE_CAP_H
