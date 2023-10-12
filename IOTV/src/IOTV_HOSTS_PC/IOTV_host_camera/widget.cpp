#include "widget.h"

#include <QMediaDevices>
#include <QCameraDevice>
#include <QCamera>
#include <QImageCapture>
#include <QMediaCaptureSession>
#include <QThread>

Widget::Widget(QObject *parent)
    : QObject(parent)
{
    timer = new QTimer(this);

    connect(&imageCapture, &QImageCapture::imageCaptured, this, &Widget::processCapturedImage);
    connect(timer, &QTimer::timeout, this, &Widget::timerOut);

    camera = new QCamera(QMediaDevices::defaultVideoInput());

    captureSession.setCamera(camera);
    captureSession.setImageCapture(&imageCapture);

    //    const QList<QCameraDevice> availableCameras = QMediaDevices::videoInputs();

    //    for (const QCameraDevice &cameraDevice : availableCameras) {

    //        qDebug() << cameraDevice;
    //        for (auto &el : cameraDevice.videoFormats())
    //            qDebug() << el.resolution() << ' ' << el.maxFrameRate() << ' ' << el.minFrameRate() << ' ' << el.pixelFormat();

    //        camera = new QCamera(cameraDevice);

    //        captureSession.setCamera(camera);
    //        captureSession.setImageCapture(&imageCapture);
    //    }

    QThread::sleep(5);
    timer->start(50);
}

Widget::~Widget()
{
}

size_t Widget::getData(char *outData, size_t outDataSize) const
{
    if (image.sizeInBytes() > outDataSize)
        return 0;

    memcpy(outData, image.bits(), image.sizeInBytes());

    return image.sizeInBytes();
}


void Widget::processCapturedImage(int requestId, const QImage &img)
{
    Q_UNUSED(requestId);

    timer->stop();
    image = img;

    qDebug() << "photo size " << img.sizeInBytes();
    //    ui->label->setPixmap(QPixmap::fromImage(img));

    timer->start(50);
}

void Widget::timerOut()
{
    imageCapture.capture();
}

