#ifndef IOT_SERVER_H
#define IOT_SERVER_H

#include <QSettings>

#include <vector>
#include <memory>

#include "IOT_Host/IOT_Host.h"

class IOT_Server : public QObject
{
    Q_OBJECT
public:
    IOT_Server(QObject *parent = nullptr);

    QString getFileSettingName() const;
    QString getProgramVersion() const;

private:
    void readSettings();

    //    void readData(const QString hostName, uint8_t channelNumber);

    std::vector<std::unique_ptr<IOT_Host>> _iot_hosts;
    QSettings _settings;
    const QString _programVersion;

signals:

};

#endif // IOT_SERVER_H
