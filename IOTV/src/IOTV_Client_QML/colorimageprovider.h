#pragma once

#include "wrap_qbytearray.h"
#include <QQuickImageProvider>
#include <qqml.h>

class ColorImageProvider : public QQuickImageProvider
{
public:
    ColorImageProvider();

    virtual QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
    void refreshImage(Wrap_QByteArray *wdata);

private:
    QImage _image;
};
