#pragma once

#include <QQuickImageProvider>
#include <qqml.h>

class ColorImageProvider : public QQuickImageProvider
{
//    Q_PROPERTY(QByteArray image WRITE slotRefreshImage)
public:
    ColorImageProvider();

    virtual QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

public slots:
    void slotRefreshImage(QByteArray byteArr);

private:
    QImage _image;
};
