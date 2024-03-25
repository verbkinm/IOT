#pragma once

#include <QAudioOutput>
#include <QAudioSink>
#include <QObject>

#include "wrap_qbytearray.h"

class GeneralAudioOutput : public QObject
{
    Q_OBJECT
public:
    explicit GeneralAudioOutput(QObject *parent = nullptr);

signals:

public slots:
    void slotWriteData(Wrap_QByteArray *wdata);

private:
    QAudioOutput *_audioOut;
    QAudioSink *_sink;
    QIODevice *devOut;
};
