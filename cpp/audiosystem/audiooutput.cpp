#include "audiooutput.h"

#include <QMediaDevices>
#include <QIODevice>
#include <QAudioSink>

AudioOutput::AudioOutput(
    const AudioDevice& audioDevice,
    const AudioOutputConfig& config,
    QObject* const parent
)
    : QObject{ parent }
    , m_config{ config }
    , m_audioDevice{ audioDevice }
{
    connect(&m_audioDevice, &AudioDevice::deviceChanged,
            this, &AudioOutput::onDeviceChanged);

    connect(&m_audioDevice, &AudioDevice::enabledChanged, this, [this](const bool enabled) {
        if (enabled) {
            start();
        } else {
            stop();
        }
    });

    const auto mediaDevices{ new QMediaDevices{ this } };
    connect(mediaDevices, &QMediaDevices::audioOutputsChanged, this, [this] {
        if (!QMediaDevices::audioOutputs().contains(m_audioDevice.device())) {
            invalidate();
        }
    });
}

bool AudioOutput::start()
{
    if (!m_audioSink || !m_audioDevice.enabled()) {
        return false;
    }

    m_ioDevice = m_audioSink->start();
    return m_audioSink->error() == QtAudio::NoError;
}

void AudioOutput::stop()
{
    if (m_ioDevice) {
        m_ioDevice->close();
        m_ioDevice = nullptr;
    }

    if (m_audioSink) {
        m_audioSink->reset();
    }
}

QAudioSink* AudioOutput::audioSink() const
{
    return m_audioSink.get();
}

QIODevice* AudioOutput::ioDevice() const
{
    if (!m_audioSink || m_audioSink->state() == QtAudio::StoppedState) {
        return nullptr;
    }
    return m_ioDevice;
}

void AudioOutput::onDeviceChanged()
{
    const auto oldFormat{ format() };
    if (oldFormat.sampleFormat() == QAudioFormat::Unknown) {
        initialize();
    } else {
        initialize(oldFormat);
    }
}

bool AudioOutput::initialize(const QAudioFormat& format)
{
    invalidate();
    if (m_audioDevice.device().isNull()) {
        return false;
    }

    if (!m_audioDevice.device().isFormatSupported(format)) {
        return false;
    }

    m_audioSink = new QAudioSink{
        m_audioDevice.device(),
        format,
        this
    };

    m_audioSink->setBufferSize(
        format.sampleRate()
        * format.bytesPerSample()
        * format.channelCount()
        * m_config.bufferedMs / 1000.f
    );

    if (m_config.initializeVolume) {
        m_audioSink->setVolume(m_audioDevice.volume());
        connect(&m_audioDevice, &AudioDevice::volumeChanged,
                m_audioSink, &QAudioSink::setVolume);
    }

    emit initialized();
    return true;
}

void AudioOutput::invalidate()
{
    if (m_ioDevice) {
        m_ioDevice->close();
        m_ioDevice = nullptr;
    }

    if (!m_audioSink) {
        return;
    }

    m_audioSink->reset();
    m_audioSink->stop();
    m_audioSink->deleteLater();
}

QAudioFormat AudioOutput::format() const
{
    if (!m_audioSink) {
        return {};
    }
    return m_audioSink->format();
}
