#ifndef WIDGET_H
#define WIDGET_H

#include <QMediaDevices>
#include <QCameraDevice>
#include <QCamera>
#include <QImageCapture>
#include <QMediaCaptureSession>
#include <QTimer>


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

    static constexpr int INTERVAL = 10;

public slots:
    void processCapturedImage(int requestId, const QImage &img);
    void timerOut();
    void errorCapture(int, QImageCapture::Error err, QString errorStr);
    void readyForCapture(bool);

signals:
    void signalFirstCapture();
    void signalImageCaptured();
};
#endif // WIDGET_H
