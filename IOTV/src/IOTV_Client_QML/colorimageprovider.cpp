#include "colorimageprovider.h"

#include <memory>
std::mutex _mutex;

ColorImageProvider::ColorImageProvider() : QQuickImageProvider(QQmlImageProviderBase::Image)
{

}

QImage ColorImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(id)

//    int width = 100;
//    int height = 50;

//    if (size)
//        *size = QSize(width, height);

//    QImage img(requestedSize.width() > 0 ? requestedSize.width() : width,
//               requestedSize.height() > 0 ? requestedSize.height() : height,
//               QImage::Format_RGBA8888);

//    _image = img;
//    img.fill(Qt::red);

    std::lock_guard lg(_mutex);
    QImage img = _image;

    return img;
}

void ColorImageProvider::refreshImage(Wrap_QByteArray *wdata)
{
    std::lock_guard lg(_mutex);
    _image.loadFromData(wdata->data(), "JPG");
}
