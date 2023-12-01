// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "audiorecorder.h"
#include "audiolevel.h"
#include "qpainter.h"
#include "ui_audiorecorder.h"

#include <QAudioBuffer>
#include <QAudioDevice>
#include <QAudioInput>
#include <QAudioOutput>
#include <QAudioSink>
#include <QAudioSource>
#include <QCamera>
#include <QVideoWidget>
#include <QVideoSink>
#include <QDir>
#include <QFileDialog>
#include <QImageCapture>
#include <QMediaDevices>
#include <QMediaFormat>
#include <QMediaRecorder>
#include <QMimeType>
#include <QStandardPaths>
#include <QDebug>

//#if QT_CONFIG(permissions)
//  #include <QPermission>
//#endif

static QList<qreal> getBufferLevels(const QAudioBuffer &buffer);

QIODevice *devIn;
QIODevice *devOut;
QVideoWidget *viewfinder;
QVideoSink *mySink;

AudioRecorder::AudioRecorder() : ui(new Ui::AudioRecorder)
{
    ui->setupUi(this);

    //    // channels
    //    ui->channelsBox->addItem(tr("Default"), QVariant(-1));
    //    ui->channelsBox->addItem(QStringLiteral("1"), QVariant(1));
    //    ui->channelsBox->addItem(QStringLiteral("2"), QVariant(2));
    //    ui->channelsBox->addItem(QStringLiteral("4"), QVariant(4));

    //    // quality
    //    ui->qualitySlider->setRange(0, int(QImageCapture::VeryHighQuality));
    //    ui->qualitySlider->setValue(int(QImageCapture::NormalQuality));

    //    // bit rates:
    //    ui->bitrateBox->addItem(tr("Default"), QVariant(0));
    //    ui->bitrateBox->addItem(QStringLiteral("32000"), QVariant(32000));
    //    ui->bitrateBox->addItem(QStringLiteral("64000"), QVariant(64000));
    //    ui->bitrateBox->addItem(QStringLiteral("96000"), QVariant(96000));
    //    ui->bitrateBox->addItem(QStringLiteral("128000"), QVariant(128000));

    //    // audio input initialization
    //    init();


    //    m_audioRecorder = new QMediaRecorder(this);
    //    m_captureSession.setRecorder(m_audioRecorder);
    //    m_captureSession.setAudioInput(new QAudioInput(QMediaDevices::defaultAudioInput(), this));
    //    m_captureSession.audioInput()->setDevice(QMediaDevices::audioInputs().at(0));

    //    QMediaFormat format;
    //    format.setFileFormat(QMediaFormat::UnspecifiedFormat);
    //    format.setAudioCodec(QMediaFormat::AudioCodec::Unspecified);

    //    m_audioRecorder->setMediaFormat(format);
    //    m_audioRecorder->setAudioSampleRate(44100);
    //    m_audioRecorder->setAudioBitRate(0);
    //    m_audioRecorder->setAudioChannelCount(1);
    //    m_audioRecorder->setQuality(QMediaRecorder::Quality::NormalQuality);
    //    m_audioRecorder->setEncodingMode(QMediaRecorder::ConstantQualityEncoding);

    //    m_audioRecorder->record();


    QAudioFormat format;
    // Set up the format, eg.
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Float);

    QAudioInput *audioIn = new QAudioInput(QMediaDevices::audioInputs().at(1));
    //    audioIn->setVolume(0);
    QAudioOutput *audioOut = new QAudioOutput(QMediaDevices::defaultAudioOutput());

    QAudioSink *sink = new QAudioSink(audioOut->device(), format);
    QAudioSource *source = new QAudioSource(audioIn->device(), format);

    devIn = source->start();
    devOut = sink->start();

    source->setVolume(0.1);

    connect(devIn, &QIODevice::readyRead, this, &AudioRecorder::slotReadyRead);
    connect(devOut, &QIODevice::bytesWritten, this, &AudioRecorder::slotBytesWriten);







    //    QMediaCaptureSession *captureSession = new QMediaCaptureSession;
    //    QCamera *camera = new QCamera;

    //    for (auto &el : camera->cameraDevice().videoFormats())
    //        qDebug() << el.resolution() << ' ' << el.maxFrameRate() << ' ' << el.minFrameRate() << ' ' << el.pixelFormat();

    ////    camera->setCameraFormat(camera->cameraDevice().videoFormats().last());
    //    captureSession->setCamera(camera);
    //    captureSession->setAudioInput(audioIn);
    //    captureSession->setAudioOutput(audioOut);

    //    viewfinder = new QVideoWidget;
    //    mySink = viewfinder->videoSink();
    //    captureSession->setVideoOutput(viewfinder);

    //    viewfinder->show();
    //    camera->start();


    //    connect(mySink, &QVideoSink::videoFrameChanged, this, &AudioRecorder::slotVideFrameChanged);

}

void AudioRecorder::init()
{
    //#if QT_CONFIG(permissions)
    //    QMicrophonePermission microphonePermission;
    //    switch (qApp->checkPermission(microphonePermission)) {
    //    case Qt::PermissionStatus::Undetermined:
    //        qApp->requestPermission(microphonePermission, this, &AudioRecorder::init);
    //        return;
    //    case Qt::PermissionStatus::Denied:
    //        qWarning("Microphone permission is not granted!");
    //        return;
    //    case Qt::PermissionStatus::Granted:
    //        break;
    //    }
    //#endif

    m_audioRecorder = new QMediaRecorder(this);
    m_captureSession.setRecorder(m_audioRecorder);
    m_captureSession.setAudioInput(new QAudioInput(this));
    // ### replace with a monitoring output once we have it.
    // m_probe = new QAudioProbe(this);
    // connect(m_probe, &QAudioProbe::audioBufferProbed,
    //         this, &AudioRecorder::processBuffer);
    // m_probe->setSource(m_audioRecorder);

    // audio devices
    ui->audioDeviceBox->addItem(tr("Default"), QVariant(QString()));
    for (auto device : QMediaDevices::audioInputs()) {
        auto name = device.description();
        ui->audioDeviceBox->addItem(name, QVariant::fromValue(device));
    }

    // audio codecs and container formats
    updateFormats();
    connect(ui->audioCodecBox, &QComboBox::currentIndexChanged, this,
            &AudioRecorder::updateFormats);
    connect(ui->containerBox, &QComboBox::currentIndexChanged, this, &AudioRecorder::updateFormats);

    // sample rate
    ui->sampleRateBox->setRange(m_captureSession.audioInput()->device().minimumSampleRate(),
                                m_captureSession.audioInput()->device().maximumSampleRate());
    ui->sampleRateBox->setValue(
        qBound(m_captureSession.audioInput()->device().minimumSampleRate(), 44100,
               m_captureSession.audioInput()->device().maximumSampleRate()));

    connect(m_audioRecorder, &QMediaRecorder::durationChanged, this,
            &AudioRecorder::updateProgress);
    connect(m_audioRecorder, &QMediaRecorder::recorderStateChanged, this,
            &AudioRecorder::onStateChanged);
    connect(m_audioRecorder, &QMediaRecorder::errorChanged, this,
            &AudioRecorder::displayErrorMessage);
}

void AudioRecorder::updateProgress(qint64 duration)
{
    if (m_audioRecorder->error() != QMediaRecorder::NoError || duration < 2000)
        return;

    ui->statusbar->showMessage(tr("Recorded %1 sec").arg(duration / 1000));
}

void AudioRecorder::onStateChanged(QMediaRecorder::RecorderState state)
{
    QString statusMessage;

    switch (state) {
    case QMediaRecorder::RecordingState:
        statusMessage = tr("Recording to %1").arg(m_audioRecorder->actualLocation().toString());
        ui->recordButton->setText(tr("Stop"));
        ui->pauseButton->setText(tr("Pause"));
        break;
    case QMediaRecorder::PausedState:
        clearAudioLevels();
        statusMessage = tr("Paused");
        ui->recordButton->setText(tr("Stop"));
        ui->pauseButton->setText(tr("Resume"));
        break;
    case QMediaRecorder::StoppedState:
        clearAudioLevels();
        statusMessage = tr("Stopped");
        ui->recordButton->setText(tr("Record"));
        ui->pauseButton->setText(tr("Pause"));
        break;
    }

    ui->pauseButton->setEnabled(m_audioRecorder->recorderState() != QMediaRecorder::StoppedState);
    if (m_audioRecorder->error() == QMediaRecorder::NoError)
        ui->statusbar->showMessage(statusMessage);
}

static QVariant boxValue(const QComboBox *box)
{
    int idx = box->currentIndex();
    if (idx == -1)
        return QVariant();

    return box->itemData(idx);
}

void AudioRecorder::toggleRecord()
{
    m_audioRecorder->stop();
    return;

    if (m_audioRecorder->recorderState() == QMediaRecorder::StoppedState) {
        m_captureSession.audioInput()->setDevice(
            boxValue(ui->audioDeviceBox).value<QAudioDevice>());

        //        m_audioRecorder->setMediaFormat(selectedMediaFormat());
        //        m_audioRecorder->setAudioSampleRate(ui->sampleRateBox->value());
        //        m_audioRecorder->setAudioBitRate(boxValue(ui->bitrateBox).toInt());
        //        m_audioRecorder->setAudioChannelCount(boxValue(ui->channelsBox).toInt());
        //        m_audioRecorder->setQuality(QMediaRecorder::Quality(ui->qualitySlider->value()));
        //        m_audioRecorder->setEncodingMode(ui->constantQualityRadioButton->isChecked()
        //                                                 ? QMediaRecorder::ConstantQualityEncoding
        //                                                 : QMediaRecorder::ConstantBitRateEncoding);

        QMediaFormat format;
        format.setFileFormat(QMediaFormat::UnspecifiedFormat);
        format.setAudioCodec(QMediaFormat::AudioCodec::Unspecified);

        m_audioRecorder->setMediaFormat(format);
        m_audioRecorder->setAudioSampleRate(44100);
        m_audioRecorder->setAudioBitRate(0);
        m_audioRecorder->setAudioChannelCount(1);
        m_audioRecorder->setQuality(QMediaRecorder::Quality::NormalQuality);
        m_audioRecorder->setEncodingMode(QMediaRecorder::ConstantQualityEncoding);
        //                                             : QMediaRecorder::ConstantBitRateEncoding);

        qDebug() << m_audioRecorder->mediaFormat().fileFormat() << m_audioRecorder->mediaFormat().audioCodec()
                 << m_audioRecorder->audioSampleRate() << m_audioRecorder->audioBitRate() << m_audioRecorder->encodingMode()
                 << m_audioRecorder->quality();
        m_audioRecorder->record();
    } else {
        m_audioRecorder->stop();
    }
}

void AudioRecorder::togglePause()
{
    if (m_audioRecorder->recorderState() != QMediaRecorder::PausedState)
        m_audioRecorder->pause();
    else
        m_audioRecorder->record();
}

void AudioRecorder::setOutputLocation()
{
#ifdef Q_OS_ANDROID
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save Recording"),
        "output." + selectedMediaFormat().mimeType().preferredSuffix());
#else
    QString fileName = QFileDialog::getSaveFileName();
#endif
    m_audioRecorder->setOutputLocation(QUrl::fromLocalFile(fileName));
    m_outputLocationSet = true;
}

void AudioRecorder::displayErrorMessage()
{
    ui->statusbar->showMessage(m_audioRecorder->errorString());
}

void AudioRecorder::updateFormats()
{
    if (m_updatingFormats)
        return;
    m_updatingFormats = true;

    QMediaFormat format;
    if (ui->containerBox->count())
        format.setFileFormat(boxValue(ui->containerBox).value<QMediaFormat::FileFormat>());
    if (ui->audioCodecBox->count())
        format.setAudioCodec(boxValue(ui->audioCodecBox).value<QMediaFormat::AudioCodec>());

    int currentIndex = 0;
    ui->audioCodecBox->clear();
    ui->audioCodecBox->addItem(tr("Default audio codec"),
                               QVariant::fromValue(QMediaFormat::AudioCodec::Unspecified));
    for (auto codec : format.supportedAudioCodecs(QMediaFormat::Encode)) {
        if (codec == format.audioCodec())
            currentIndex = ui->audioCodecBox->count();
        ui->audioCodecBox->addItem(QMediaFormat::audioCodecDescription(codec),
                                   QVariant::fromValue(codec));
    }
    ui->audioCodecBox->setCurrentIndex(currentIndex);

    currentIndex = 0;
    ui->containerBox->clear();
    ui->containerBox->addItem(tr("Default file format"),
                              QVariant::fromValue(QMediaFormat::UnspecifiedFormat));
    for (auto container : format.supportedFileFormats(QMediaFormat::Encode)) {
        if (container < QMediaFormat::Mpeg4Audio) // Skip video formats
            continue;
        if (container == format.fileFormat())
            currentIndex = ui->containerBox->count();
        ui->containerBox->addItem(QMediaFormat::fileFormatDescription(container),
                                  QVariant::fromValue(container));
    }
    ui->containerBox->setCurrentIndex(currentIndex);

    m_updatingFormats = false;
}

void AudioRecorder::slotBytesWriten(qint64 bytes)
{
    qDebug() << "written " << bytes;
}

void AudioRecorder::slotReadyRead()
{
    qDebug() << devOut->write(devIn->readAll());
}

void AudioRecorder::slotVideFrameChanged(const QVideoFrame &frame)
{
    QVideoFrame vFrame(frame);

    vFrame.map(QVideoFrame::ReadOnly);

    qDebug() << frame.bytesPerLine(0) << frame.mappedBytes(0);

    auto data = vFrame.bits(0);

    //    for (int i = 0; i < vFrame.mappedBytes(0); ++i)
    //    {
    //        data[i] = 1;
    //    }
    //    vFrame.setMirrored(true);
    vFrame.setSubtitleText(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz"));
    vFrame.unmap();
}

void AudioRecorder::clearAudioLevels()
{
    for (auto m_audioLevel : std::as_const(m_audioLevels))
        m_audioLevel->setLevel(0);
}

QMediaFormat AudioRecorder::selectedMediaFormat() const
{
    QMediaFormat format;
    format.setFileFormat(boxValue(ui->containerBox).value<QMediaFormat::FileFormat>());
    format.setAudioCodec(boxValue(ui->audioCodecBox).value<QMediaFormat::AudioCodec>());
    return format;
}

// returns the audio level for each channel
QList<qreal> getBufferLevels(const QAudioBuffer &buffer)
{
    QList<qreal> values;

    auto format = buffer.format();
    if (!format.isValid())
        return values;

    int channels = buffer.format().channelCount();
    values.fill(0, channels);

    int bytesPerSample = format.bytesPerSample();
    QList<qreal> max_values;
    max_values.fill(0, channels);

    const char *data = buffer.constData<char>();
    for (int i = 0; i < buffer.frameCount(); ++i) {
        for (int j = 0; j < channels; ++j) {
            qreal value = qAbs(format.normalizedSampleValue(data));
            if (value > max_values.at(j))
                max_values[j] = value;
            data += bytesPerSample;
        }
    }

    return max_values;
}

void AudioRecorder::processBuffer(const QAudioBuffer &buffer)
{
    if (m_audioLevels.count() != buffer.format().channelCount()) {
        qDeleteAll(m_audioLevels);
        m_audioLevels.clear();
        for (int i = 0; i < buffer.format().channelCount(); ++i) {
            AudioLevel *level = new AudioLevel(ui->centralwidget);
            m_audioLevels.append(level);
            ui->levelsLayout->addWidget(level);
        }
    }

    QList<qreal> levels = getBufferLevels(buffer);
    for (int i = 0; i < levels.count(); ++i)
        m_audioLevels.at(i)->setLevel(levels.at(i));
}

