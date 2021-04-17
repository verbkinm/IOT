#ifndef SHEDULE_H
#define SHEDULE_H

#include <QTimer>

class Shedule : public QObject
{
    Q_OBJECT
public:
    Shedule(const char RW, uint8_t channelNumber, int interval,
            const QString& fileName, QObject *parent = nullptr);

    const char RW;
    const uint8_t channelNumber;
    const QString fileName;

private:
    QTimer _timer;

signals:
    void signalTimeOut();

};

#endif // SHEDULE_H
