#include "producer.h"
//#include "fstream"

Producer::Producer(QObject *parent):QObject(parent), _mirrored(false)
{
    QAudioFormat format;
    // Set up the format, eg.
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int32);

    QAudioOutput *audioOut = new QAudioOutput(QMediaDevices::defaultAudioOutput(), this);
    QAudioSink *sink = new QAudioSink(audioOut->device(), format, this);

    devOut = sink->start();
    audioOut->setVolume(1.0);
}

QVideoSink *Producer::videoSink() const
{
    return m_videoSink.get();
}

void Producer::setVideoSink(QVideoSink *newVideoSink)
{
    if (m_videoSink == newVideoSink)
        return;

    m_videoSink = newVideoSink;
    emit videoSinkChanged();
}

void Producer::handleTimeout()
{
    if(!m_videoSink)
        return;
    QVideoFrame video_frame(QVideoFrameFormat(QSize(640, 480),QVideoFrameFormat::Format_BGRA8888));
    if(!video_frame.isValid() || !video_frame.map(QVideoFrame::WriteOnly)){
        qWarning() << "QVideoFrame is not valid or not writable";
        return;
    }
    QImage::Format image_format = QVideoFrameFormat::imageFormatFromPixelFormat(video_frame.pixelFormat());
    if(image_format == QImage::Format_Invalid){
        qWarning() << "It is not possible to obtain image format from the pixel format of the videoframe";
        return;
    }
    int plane = 0;
    QImage image(video_frame.bits(plane), video_frame.width(),video_frame.height(), image_format);
    image.fill(QColor::fromRgb(QRandomGenerator::global()->generate()));
    QPainter painter(&image);
    painter.drawText(image.rect(), Qt::AlignCenter, QDateTime::currentDateTime().toString());
    painter.end();

    video_frame.unmap();
    m_videoSink->setVideoFrame(video_frame);
}

void Producer::slotDataVideoFrame(int w, int h, Wrap_QByteArray *data)
{
    qDebug() << "data" << w << h << data->data().size();

    if (w == 0 || h == 0 || data == nullptr)
        return;

//    std::ofstream file("image2.jpg");
//    file.write(data->data().data(), data->data().size());

    QByteArray ba = data->data();
    QBuffer buf(&ba);
    buf.open(QIODevice::ReadOnly);

    QImage img;
    img.loadFromData(ba);
//    img.save("image.jpg");


    QVideoFrame video_frame(QVideoFrameFormat(img.size(), QVideoFrameFormat::pixelFormatFromImageFormat(img.format())));

    if(!video_frame.isValid() || !video_frame.map(QVideoFrame::WriteOnly)){
        qWarning() << "QVideoFrame is not valid or not writable";
        return;
    }

    uchar *bits = video_frame.bits(0);
    std::memcpy(bits, img.bits(), img.sizeInBytes());
    video_frame.unmap();

    if(!video_frame.isValid()){
        qWarning() << "QVideoFrame is not valid or not writable";
        return;
    }

    if (_mirrored)
        video_frame.setMirrored(_mirrored);

    m_videoSink->setVideoFrame(video_frame);
}

void Producer::slotDataAudioFrame(Wrap_QByteArray *data)
{
    qDebug() << "write audio data";
    devOut->write(data->data());
}

void Producer::slotMirrored(bool val)
{
    _mirrored = val;
}
