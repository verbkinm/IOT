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

    size_t getData(char *outData, size_t outDataSize) const;

private:
    QCamera *camera;
    QMediaCaptureSession captureSession;
    QImageCapture imageCapture;
    QTimer *timer;
    QImage image;

private slots:
    void processCapturedImage(int requestId, const QImage &img);
    void timerOut();
};
#endif // WIDGET_H
