#ifndef PRODUCER_H
#define PRODUCER_H

#include <QObject>
#include <QPointer>
#include <QVideoSink>
#include <QQmlEngine>
#include <QImage>
#include <QPainter>
#include <QSize>
#include <QVideoFrame>
#include <QBuffer>

#include <QAudioDevice>
#include <QAudioOutput>
#include <QAudioSink>
#include <QAudioSource>
#include <QMediaDevices>
#include <QMediaFormat>
#include <QImageCapture>

#include <QRandomGenerator>
#include <QDateTime>

#include "qmediacapturesession.h"
#include "qmediarecorder.h"

#include "wrap_qbytearray.h"
#include "device.h"

class Producer : public QObject
{
    Q_OBJECT
//    QML_ELEMENT
    Q_PROPERTY(QVideoSink* videoSink READ videoSink WRITE setVideoSink NOTIFY videoSinkChanged)
    Q_PROPERTY(Device* video_device READ getDevice WRITE setDevice NOTIFY videoDeviceChanged)
//    Q_PROPERTY(QAudioSink* audioSink READ getAudioDevice WRITE setAudioDevice NOTIFY audioDeviceChanged)


public:
    explicit Producer(QObject *parent = nullptr);
    ~Producer();

    QVideoSink *videoSink() const;
    void setVideoSink(QVideoSink *newVideoSink);

    Device *getDevice();
    void setDevice(Device *dev);

//    QAudioSink *getAudioDevice();
//    void setAudioDevice(QAudioSink *dev);

private:
    QPointer<QVideoSink> m_videoSink;
    QImageCapture *imageCapture;
    QIODevice *devOut;
    QAudioOutput *_audioOut;
    QAudioSink *_audioSink;

    QMediaCaptureSession *session;
    QMediaRecorder *recorder;

    Device *_device;

    bool _mirrored;

    void handleTimeout();

signals:
    void videoSinkChanged();
    void videoDeviceChanged();
    void audioDeviceChanged();

public slots:
    void slotDataPkgComplete(int channel, QByteArray data);
    void slotDataVideoFrame(int w, int h, const QByteArray &data);
    void slotDataAudioFrame(Wrap_QByteArray *data);

    void slotMirrored(bool val);
};

#endif // PRODUCER_H
