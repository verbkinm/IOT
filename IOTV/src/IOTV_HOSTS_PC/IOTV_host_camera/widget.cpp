#include "widget.h"
#include "qbuffer.h"
#include "qurl.h"

#include <QMediaDevices>
#include <QCameraDevice>
#include <QCamera>
#include <QImageCapture>
#include <QMediaCaptureSession>

Widget::Widget(QObject *parent)
    : QObject(parent), /*_format(QMediaFormat::MPEG4),*/ _image(nullptr)
{
    timer = new QTimer(this);

//    connect(&imageCapture, &QImageCapture::readyForCaptureChanged, this, &Widget::readyForCapture);
    connect(&imageCapture, &QImageCapture::imageCaptured, this, &Widget::processCapturedImage);
    connect(&imageCapture, &QImageCapture::errorOccurred, this, &Widget::errorCapture);
//    connect(&imageCapture, &QImageCapture::imageAvailable, this, &Widget::processCapturedFrame);
    connect(timer, &QTimer::timeout, this, &Widget::timerOut, Qt::QueuedConnection);

    connect(&imageCapture, &QImageCapture::imageCaptured, this, &Widget::signalFirstCapture, Qt::SingleShotConnection);


    camera = new QCamera(QMediaDevices::defaultVideoInput());
    camera->start();

    captureSession.setCamera(camera);
    captureSession.setImageCapture(&imageCapture);
//    captureSession.setRecorder(&_recorder);

//    _format.setAudioCodec(QMediaFormat::AudioCodec::AAC);
//    _format.setVideoCodec(QMediaFormat::VideoCodec::H265);
//    _recorder.setMediaFormat(_format);
//    _recorder.setOutputLocation(QUrl("mycapture.mp4"));
//    _recorder.record();

    const QList<QCameraDevice> availableCameras = QMediaDevices::videoInputs();

    for (const QCameraDevice &cameraDevice : availableCameras)
    {
        qDebug() << cameraDevice;
        for (auto &el : cameraDevice.videoFormats())
            qDebug() << el.resolution() << ' ' << el.maxFrameRate() << ' ' << el.minFrameRate() << ' ' << el.pixelFormat();
    }

//    timer->start(INTERVAL);
    imageCapture.capture();
}

Widget::~Widget()
{

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

    if (!timer->isActive())
        return;

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

