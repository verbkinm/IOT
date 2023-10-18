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

    connect(&imageCapture, &QImageCapture::readyForCaptureChanged, this, &Widget::readyForCapture);
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

        //        camera = new QCamera(cameraDevice);

        //        captureSession.setCamera(camera);
        //        captureSession.setImageCapture(&imageCapture);
    }

    //    QThread::sleep(5);
//    timer->start(100);
    imageCapture.capture();

}

Widget::~Widget()
{

}

const QImage &Widget::getImage() const
{
    return *_image;
}

size_t Widget::getImageSavedSize() const
{
    if (_image == nullptr)
        return 0;

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    _image->save(&buffer, "JPG");

    auto s = ba.size();

    return ba.size();
}


void Widget::processCapturedImage(int requestId, const QImage &img)
{
    Q_UNUSED(requestId);

    timer->stop();

    if (_image != nullptr)
        delete _image;

    _image = new QImage(img);

    _image->save("Image.jpg", "JPG");

    qDebug() << _image->format();

//    auto s = getImageSavedSize();

//    qDebug() << img.size();
//    qDebug() << "photo size " << img.sizeInBytes();
    //    ui->label->setPixmap(QPixmap::fromImage(img));

    timer->start(100);
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
    timer->start(100);
}

