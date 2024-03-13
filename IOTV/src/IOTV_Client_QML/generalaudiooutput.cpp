#include "generalaudiooutput.h"

#include <QMediaDevices>

GeneralAudioOutput::GeneralAudioOutput(QObject *parent)
    : QObject{parent}
{
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Float);

    _audioOut = new QAudioOutput(QMediaDevices::defaultAudioOutput());
    _sink = new QAudioSink(_audioOut->device(), format);

    devOut = _sink->start();
    _sink->setVolume(1.0);
}

void GeneralAudioOutput::slotWriteData(Wrap_QByteArray *wdata)
{
    devOut->write(wdata->data());
    delete wdata;
}
