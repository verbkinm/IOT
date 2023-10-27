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

//            connect(&imageCapture, &QImageCapture::readyForCaptureChanged, this, &Widget::readyForCapture);
        connect(&imageCapture, &QImageCapture::imageCaptured, this, &Widget::processCapturedImage);
        connect(&imageCapture, &QImageCapture::errorOccurred, this, &Widget::errorCapture);
        //    connect(&imageCapture, &QImageCapture::imageAvailable, this, &Widget::processCapturedFrame);
        connect(timer, &QTimer::timeout, this, &Widget::timerOut, Qt::QueuedConnection);

        connect(&imageCapture, &QImageCapture::imageCaptured, this, &Widget::signalFirstCapture, Qt::SingleShotConnection);


        camera = new QCamera(/*QMediaDevices::videoInputs().at(1)*/QMediaDevices::defaultVideoInput());
        camera->start();

        captureSession.setCamera(camera);
        captureSession.setImageCapture(&imageCapture);
    //    //    captureSession.setRecorder(&_recorder);


    //    audioInput = new QAudioInput(QMediaDevices::defaultAudioInput());
//    recorder = new QMediaRecorder(this);
//    session.setRecorder(recorder);
//    session.setAudioInput(new QAudioInput(this));
//    session.audioInput()->setDevice(QMediaDevices::audioInputs().at(0));

//    QMediaFormat format;
//    format.setFileFormat(QMediaFormat::UnspecifiedFormat);
//    format.setAudioCodec(QMediaFormat::AudioCodec::Unspecified);

//    recorder->setMediaFormat(format);
//    recorder->setAudioSampleRate(44100);
//    recorder->setAudioBitRate(0);
//    recorder->setAudioChannelCount(1);
//    recorder->setQuality(QMediaRecorder::Quality::NormalQuality);
//    recorder->setEncodingMode(QMediaRecorder::ConstantQualityEncoding);


//    //    connect(recorder, &QMediaRecorder::recorderStateChanged, this,
//    //            &Widget::onStateChanged);
//    connect(recorder, &QMediaRecorder::errorChanged, this,
//            &Widget::displayErrorMessage);

//    recorder->record();

    //    const QList<QCameraDevice> availableCameras = QMediaDevices::videoInputs();

    //    for (const QCameraDevice &cameraDevice : availableCameras)
    //    {
    //        qDebug() << cameraDevice;
    //        for (auto &el : cameraDevice.videoFormats())
    //            qDebug() << el.resolution() << ' ' << el.maxFrameRate() << ' ' << el.minFrameRate() << ' ' << el.pixelFormat();
    //    }

    //    audioOut = new QAudioOutput(QMediaDevices::defaultAudioOutput());


    //    desiredFormat.setChannelCount(2);
    //    desiredFormat.setSampleFormat(QAudioFormat::Int16);
    //    desiredFormat.setSampleRate(48000);

    //    decoder = new QAudioDecoder(this);
    //    decoder->setAudioFormat(desiredFormat);
    //    decoder->setSource(QUrl::fromLocalFile("/home/user/file.mp3"));

    //    connect(decoder, &QAudioDecoder::bufferReady, this, &Widget::onAudioReadyRead);
    //    decoder->start();

//        timer->start(INTERVAL);
        imageCapture.capture();

//    QAudioFormat format;
//    // Set up the format, eg.
//    format.setSampleRate(48000);
//    format.setChannelCount(2);
//    format.setSampleFormat(QAudioFormat::Float);

//    QAudioInput *audioIn = new QAudioInput(QMediaDevices::audioInputs().at(1));
//    //    audioIn->setVolume(0);
//    QAudioOutput *audioOut = new QAudioOutput(QMediaDevices::defaultAudioOutput());

//    QAudioSink *sink = new QAudioSink(audioOut->device(), format);
//    QAudioSource *source = new QAudioSource(audioIn->device(), format);

//    //    auto dev = ;
//    sink->start(source->start());

//    source->setVolume(0.5);
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
}

void Widget::stop()
{
    timer->stop();
}


void Widget::processCapturedImage(int requestId, const QImage &img)
{
    Q_UNUSED(requestId);

//    if (!timer->isActive())
//        return;

    timer->stop();

    _image = img;
    _image.save("Image.jpg", "JPG", 100);

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    _image.save(&buffer, "JPG", 100);

    emit signalImageCaptured();

    //    imageCapture.capture();

    //    QImage newImage;
    //    newImage.loadFromData(ba, "JPG");
    //    newImage.save("Image_raw.jpg", "JPG", 50);

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

void Widget::onAudioReadyRead()
{
    auto data = decoder->read();
}

void Widget::displayErrorMessage()
{
    qDebug() << recorder->errorString();
}

