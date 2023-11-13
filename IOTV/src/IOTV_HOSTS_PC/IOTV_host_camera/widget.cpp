#include "widget.h"
#include "qbuffer.h"
#include "qmediarecorder.h"

#include <QMediaDevices>
#include <QCameraDevice>
#include <QCamera>
#include <QImageCapture>
#include <QMediaCaptureSession>
#include <QMediaPlayer>

#include <QAudioInput>
#include <QAudioOutput>
#include <QAudioSink>
#include <QAudioSource>

#include <QAudioDevice>
#include <QAudioInput>
#include <QUrl>
#include <QAudioDecoder>

Widget::Widget(QObject *parent)
    : QObject(parent), /*_format(QMediaFormat::MPEG4),*/ _image(nullptr)
{
    timer = new QTimer(this);

    connect(&imageCapture, &QImageCapture::imageCaptured, this, &Widget::processCapturedImage);
    connect(&imageCapture, &QImageCapture::errorOccurred, this, &Widget::errorCapture);
    //    connect(&imageCapture, &QImageCapture::imageAvailable, this, &Widget::processCapturedFrame);
    connect(timer, &QTimer::timeout, this, &Widget::timerOut, Qt::QueuedConnection);

    connect(&imageCapture, &QImageCapture::imageCaptured, this, &Widget::signalFirstCapture, Qt::SingleShotConnection);


    camera = new QCamera(/*QMediaDevices::videoInputs().at(1)*/QMediaDevices::defaultVideoInput());
    camera->start();

    captureSession.setCamera(camera);
    captureSession.setImageCapture(&imageCapture);

    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Float);

    audioIn = new QAudioInput(QMediaDevices::audioInputs().at(1));
    source = new QAudioSource(audioIn->device(), format);
    source->setVolume(1);
    devIn = source->start();

    imageCapture.capture();
}

Widget::~Widget()
{
    qDebug() << "destruct";
    recorder->stop();
}

const QImage Widget::getImage() const
{
    return _image;
}

size_t Widget::getImageSavedSize() const
{

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    _image.save(&buffer, "JPG");

    return ba.size();
}

void Widget::start()
{
    timer->start(INTERVAL);
    connect(devIn, &QIODevice::readyRead, this, &Widget::slotReadyRead);
}

void Widget::stop()
{
    timer->stop();
    disconnect(devIn, &QIODevice::readyRead, this, &Widget::slotReadyRead);
}


void Widget::processCapturedImage(int requestId, const QImage &img)
{
    Q_UNUSED(requestId);

    timer->stop();

    _image = img;
    _image.save("Image.jpg", "JPG", 100);

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    _image.save(&buffer, "JPG", 100);

    emit signalImageCaptured();
    timer->start(INTERVAL);
}

void Widget::processCapturedFrame(int requestId, const QVideoFrame &frame)
{
    //    Q_UNUSED(requestId);
    //    frame.i
    qDebug() << requestId << frame << frame.isValid() << frame.isMapped() << frame.isReadable()
             << frame.isWritable() << frame.startTime() << frame.endTime();
}

void Widget::timerOut()
{
    if (imageCapture.isReadyForCapture())
    {
        //        camera->stop();
        //        imageCapture.setQuality(QImageCapture::LowQuality);
        //        imageCapture.setFileFormat(QImageCapture::FileFormat::JPEG);
        //        imageCapture.setResolution(320, 240);
        //        camera->start();

        imageCapture.capture();
    }
    else
        camera->setActive(true);
}

void Widget::errorCapture(int, QImageCapture::Error err, QString errorStr)
{
    qDebug() << err << ' ' << errorStr;
}

void Widget::readyForCapture(bool)
{
    imageCapture.capture();
    qDebug() << "ready";
    timer->start(INTERVAL);
}

void Widget::displayErrorMessage()
{
    qDebug() << recorder->errorString();
}

void Widget::slotReadyRead()
{
    emit signalAudio(devIn->readAll());
}

