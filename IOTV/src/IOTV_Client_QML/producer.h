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

class Producer : public QObject
{
    Q_OBJECT
//    QML_ELEMENT
    Q_PROPERTY(QVideoSink* videoSink READ videoSink WRITE setVideoSink NOTIFY videoSinkChanged)

public:
    explicit Producer(QObject *parent = nullptr);
    ~Producer();

    QVideoSink *videoSink() const;
    void setVideoSink(QVideoSink *newVideoSink);

private:
    QPointer<QVideoSink> m_videoSink;
    QImageCapture *imageCapture;
    QIODevice *devOut;

    QMediaCaptureSession *session;
    QMediaRecorder *recorder;

    bool _mirrored;

    void handleTimeout();

signals:
    void videoSinkChanged();

public slots:
    void slotDataVideoFrame(int w, int h, Wrap_QByteArray *data);
    void slotDataAudioFrame(Wrap_QByteArray *data);

    void slotMirrored(bool val);
};

#endif // PRODUCER_H
