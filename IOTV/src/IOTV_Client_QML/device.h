#pragma once

#include <QTimer>
#include <QLineSeries>

#include "base_host.h"
#include "IOTV_SH.h"
#include "raw.h"
#include "iotv_server_embedded.h"
#include "wrap_qbytearray.h"

//struct Log_Data_Buff {
//    uint64_t timeMS;
//    QString data;
//    uint8_t flags;
//};

class Device : public Base_Host
{
    Q_OBJECT

    Q_PROPERTY(bool state READ isOnline NOTIFY signalStateChanged)
    Q_PROPERTY(int id READ getId CONSTANT)
    Q_PROPERTY(QString name READ getName CONSTANT)
    Q_PROPERTY(QString description READ getDescription CONSTANT)
    Q_PROPERTY(QString aliasName READ aliasName WRITE setAliasName NOTIFY signalAliasNameChanged)

    Q_PROPERTY(int readChannelLength READ getReadChannelLength CONSTANT)
    Q_PROPERTY(int writeChannelLength READ getWriteChannelLength CONSTANT)

public:
    Device() = default;
    explicit Device(const struct IOTV_Server_embedded *dev, QObject *parent = nullptr);
    void update(const struct IOTV_Server_embedded *dev);

    virtual QString getName() const override;

    bool isOnline() const;
    void setState(bool newState);

    bool setData(uint8_t channelNumber, const QByteArray &data);
    bool addData(uint8_t channelNumber, const QByteArray &data);
    void clearData(uint8_t channelNumber);

    Q_INVOKABLE void setDataFromString(int channelNumber, QString data);
    Q_INVOKABLE QString readData(int channelNumber) const;
    Q_INVOKABLE QString readDataType(int channelNumber) const;
    Q_INVOKABLE QString writeDataType(int channelNumber) const;
    Q_INVOKABLE void setLedColorManual(uint8_t ledNumder, bool red, bool green, bool blue);

    Q_INVOKABLE void seriesAddData(QLineSeries *series, uint8_t channelNumber, uint8_t flags);

    void setReadInterval(int interval);

    friend bool operator==(const Device &lhs, const Device &rhs);
    friend bool operator!=(const Device &lhs, const Device &rhs); // для android версий

    const QString &aliasName() const;
    void setAliasName(const QString &newAliasName);

    void addLogData(const QByteArray &data, uint8_t channelNumber);
    void clearLogData(uint8_t channelNumber);

    QByteArray logData(uint8_t channelNumber) const;

    Q_INVOKABLE void testFunc(Wrap_QByteArray *data);

private:
    const QString _name;
    QString _aliasName;

    QTimer _timerRead, _timerState;

    std::unordered_map<uint8_t, QByteArray> _log_data_buff;

signals:
    void signalQueryIdentification();
    void signalQueryRead();
    void signalQueryState();
    void signalQueryWrite(int channelNumber, QByteArray data);
    void signalQueryLogData(uint64_t startInterval, uint64_t endInterval, uint32_t interval, uint8_t channelNumber, LOG_DATA_FLAGS flags);
    // Посылается из клиента
    void signalResponceLogData(uint8_t channelNumber);
//    void signalResponceLogData(QString data, int64_t timeMS, uint8_t channelNumber, LOG_DATA_FLAGS flags);

    void signalStateChanged();
    void signalUpdate();
    void signalAliasNameChanged();

    void signalDataChanged(int channel);
    void signalDataAdded(int channel);

    void signalDataPkgComplete(int channel, const QByteArray &data);

    void signalTest(Wrap_QByteArray *data);

    void signalOpenReadStream(int channel);
    void signalCloseReadStream(int channel);

private slots:
    void slotTimerReadTimeOut();
    void slotTimerStateTimeOut();
};

