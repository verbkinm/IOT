#ifndef WIDGET_H
#define WIDGET_H

#include "qaudioinput.h"
#include "qmediaplayer.h"
#include "qmediarecorder.h"
#include <QMediaDevices>
#include <QCameraDevice>
#include <QCamera>
#include <QImageCapture>
#include <QMediaCaptureSession>
#include <QMediaFormat>
#include <QTimer>
#include <QAudioDecoder>
#include <QAudioOutput>
#include <QAudioSink>
#include <QFile>
#include <QThread>
class Widget : public QObject
{
    Q_OBJECT

public:
    Widget(QObject *parent = nullptr);
    ~Widget();

    const QImage getImage() const;
    size_t getImageSavedSize() const;

    void start();
    void stop();

private:
    QCamera *camera;
    QMediaCaptureSession captureSession;
    QImageCapture imageCapture;
    QTimer *timer;
    QImage _image;


    QMediaRecorder *recorder;
    QMediaCaptureSession session;
    QAudioInput *audioInput;

    QAudioFormat desiredFormat;
    QAudioDecoder *decoder;
    QAudioOutput *audioOut;
    QAudioSink *audioSink;
    QMediaPlayer player;

    QFile sourceFile;


    static constexpr int INTERVAL = 10;

public slots:
    void processCapturedImage(int requestId, const QImage &img);
    void processCapturedFrame(int requestId, const QVideoFrame &frame);
    void timerOut();
    void errorCapture(int, QImageCapture::Error err, QString errorStr);
    void readyForCapture(bool);

    void onAudioReadyRead();//


    void displayErrorMessage();

signals:
    void signalFirstCapture();
    void signalImageCaptured();
};
#endif // WIDGET_H
