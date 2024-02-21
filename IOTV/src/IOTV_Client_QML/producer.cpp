#include "producer.h"
#include "qmediarecorder.h"


Producer::Producer(QObject *parent):QObject(parent), _mirrored(false)
{
//        QAudioFormat audio_format;
//        // Set up the format, eg.
//        audio_format.setSampleRate(44100);
//        audio_format.setChannelCount(1);
//        audio_format.setSampleFormat(QAudioFormat::Int32);

//        QAudioOutput *audioOut = new QAudioOutput(QMediaDevices::defaultAudioOutput(), this);
//        QAudioSink *sink = new QAudioSink(audioOut->device(), audio_format, this);

//        devOut = sink->start();
//        audioOut->setVolume(1.0);


    session = new QMediaCaptureSession;
    session->setVideoSink(m_videoSink);
    //    session->setCamera(camera);
    recorder = new QMediaRecorder(this);//(camera);
    session->setRecorder(recorder);

    //    camera->start();

    // setup output format for the recorder
    QMediaFormat format;
    format.setVideoCodec(QMediaFormat::VideoCodec::H264);
    //    format.setAudioCodec(QMediaFormat::AudioCodec::MP3);
    format.setFileFormat(QMediaFormat::AVI);
    //    recorder->setVideoResolution({1280, 720});
    //    recorder->setVideoBitRate(3800);
    //    recorder->setVideoFrameRate(0);
    //    recorder->setQuality(QMediaRecorder::VeryHighQuality);
    recorder->setMediaFormat(format);

    //on shutter button pressed
    recorder->record();

}

Producer::~Producer()
{
    recorder->stop();
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
    if (w == 0 || h == 0 || data == nullptr)
        return;

    qDebug() << "data" << w << h << data->data().size();

    //    std::ofstream file("image2.jpg");
    //    file.write(data->data().data(), data->data().size());

    QByteArray ba = data->data();
    QBuffer buf(&ba);
    buf.open(QIODevice::ReadOnly);

    QImage img;
    img.loadFromData(ba);

//    imageCapture->imageCaptured(0, img);


//    int _w = img.width();
//    int _h = img.height();

//    for (int y = 0; y < _h; ++y)
//    {
//        for(int x = 0; x < _w; ++x)
//        {
//            QColor pixel = img.pixel(x, y);
//            int newVal = std::clamp(pixel.hue() + 0, 0, 255);
//            pixel.setHsl(pixel.hue(), pixel.saturation(), pixel.lightness() , pixel.alpha());
//            img.setPixel(x, y, pixel.rgba());
//        }
//    }


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

//    imageCapture->imageAvailable(0, video_frame);
}

void Producer::slotDataAudioFrame(Wrap_QByteArray *data)
{
//    qDebug() << "write audio data";
//    devOut->write(data->data());
}

void Producer::slotMirrored(bool val)
{
    _mirrored = val;
}
