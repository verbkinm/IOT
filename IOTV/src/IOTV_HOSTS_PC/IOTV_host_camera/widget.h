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

    char *getData(char *outData, size_t outDataSize) const;
    void moveTh(QThread *th);

    QImage getImage() const;

private:
    QCamera *camera;
    QMediaCaptureSession captureSession;
    QImageCapture imageCapture;
    QTimer *timer;
    QImage _image;

public slots:
    void processCapturedImage(int requestId, const QImage &img);
    void timerOut();
    void errorCapture(int, QImageCapture::Error err, QString errorStr);
    void readyForCapture(bool);
};
#endif // WIDGET_H
