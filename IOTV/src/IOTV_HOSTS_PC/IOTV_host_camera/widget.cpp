#include "widget.h"
#include "qbuffer.h"

#include <QMediaDevices>
#include <QCameraDevice>
#include <QCamera>
#include <QImageCapture>
#include <QMediaCaptureSession>
#include <QThread>

Widget::Widget(QObject *parent)
    : QObject(parent), _image(nullptr)
{
    timer = new QTimer(this);

//    connect(&imageCapture, &QImageCapture::readyForCaptureChanged, this, &Widget::readyForCapture);
    connect(&imageCapture, &QImageCapture::imageCaptured, this, &Widget::processCapturedImage);
    connect(&imageCapture, &QImageCapture::errorOccurred, this, &Widget::errorCapture);
    connect(timer, &QTimer::timeout, this, &Widget::timerOut, Qt::QueuedConnection);

    connect(&imageCapture, &QImageCapture::imageCaptured, this, &Widget::signalFirstCapture, Qt::SingleShotConnection);


    camera = new QCamera(QMediaDevices::defaultVideoInput());
    captureSession.setCamera(camera);
    captureSession.setImageCapture(&imageCapture);

    const QList<QCameraDevice> availableCameras = QMediaDevices::videoInputs();

    for (const QCameraDevice &cameraDevice : availableCameras)
    {
        qDebug() << cameraDevice;
        for (auto &el : cameraDevice.videoFormats())
            qDebug() << el.resolution() << ' ' << el.maxFrameRate() << ' ' << el.minFrameRate() << ' ' << el.pixelFormat();
    }

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

