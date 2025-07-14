#include "soundeffect.h"

#include "audiodevice.h"
#include "audioshared.h"

#include <QAudioFormat>
#include <QAudioDecoder>
#include <QAudioSink>
#include <QTimer>
#include <QMediaDevices>

SoundEffect::SoundEffect(
    const AudioDevice& outputAudioDevice,
    const AudioDevice& monitorAudioDevice
)
    : QObject{ nullptr }
    , m_outputAudioDevice{ outputAudioDevice }
    , m_monitorAudioDevice{ monitorAudioDevice }
{
    m_decoder = new QAudioDecoder{ this };
    m_decoder->setAudioFormat(AudioShared::getAudioFormat());

    connect(this, &SoundEffect::stopRequested,
            this, &SoundEffect::stop);

    connect(&m_outputAudioDevice, &AudioDevice::enabledChanged, this, [this](const bool enabled) {
        if (!enabled) {
            stop();
        }
    });
    connect(&m_monitorAudioDevice, &AudioDevice::enabledChanged, this, [this](const bool enabled) {
        if (!m_monitorAudioSink) {
            return;
        }

        if (enabled) {
            m_monitorIODevice = m_monitorAudioSink->start();
        } else {
            m_monitorAudioSink->reset();
        }
    });

    const auto mediaDevices{ new QMediaDevices{ this } };
    connect(mediaDevices, &QMediaDevices::audioOutputsChanged, this, [this] {
        if (!QMediaDevices::audioOutputs().contains(m_outputAudioDevice.device())) {
            invalidateAudioOutputSink();
        }
        if (!QMediaDevices::audioOutputs().contains(m_monitorAudioDevice.device())) {
            invalidateAudioMonitorSink();
        }
    });

    connect(&m_outputAudioDevice, &AudioDevice::deviceChanged,
            this, &SoundEffect::initAudioOutputSink);
    connect(&m_monitorAudioDevice, &AudioDevice::deviceChanged,
            this, &SoundEffect::initAudioMonitorSink);
    initAudioOutputSink();
    initAudioMonitorSink();

    m_thread.start();
    this->moveToThread(&m_thread);
}

SoundEffect::~SoundEffect()
{
    if (m_thread.isRunning()) {
        m_thread.quit();
        m_thread.wait();
    }
}

void SoundEffect::play(const QUrl& filePath)
{
    QMetaObject::invokeMethod(this, [this, filePath]() {
        if (!m_outputAudioSink || !m_outputAudioDevice.enabled()) {
            return;
        }

        // Reject last decoder buffer to prevent ASSERT: "!m_audioRenderer->isStepForced()"
        // that happens when using the ffmpeg backend
        if (m_decoder->bufferAvailable()) {
            static_cast<void>(m_decoder->read());
        }
        m_decoder->setSource(filePath);
        m_decoder->start();

        m_outputIODevice = m_outputAudioSink->start();
        processBuffer();
    }, Qt::QueuedConnection);
}

void SoundEffect::stop()
{
    if (!m_outputAudioSink || !m_outputIODevice) {
        return;
    }

    m_decoder->stop();
    m_outputAudioSink->reset();
    m_outputAudioSink->stop();
    m_outputIODevice = nullptr;
}

void SoundEffect::processBuffer()
{
    if (!m_bytesWritten) {
        if (!m_decoder->bufferAvailable()) {
            return QTimer::singleShot(1, this, &SoundEffect::processBuffer);
        }
        m_currentBuffer = m_decoder->read();
    }

    if (!m_outputAudioSink) {
        return;
    }
    const qint64 bytesToWrite{ std::min(
        m_outputAudioSink->bytesFree(), m_currentBuffer.byteCount() - m_bytesWritten
    ) };

    if (!m_outputAudioSink->bytesFree()) {
        return QTimer::singleShot(50, this, &SoundEffect::processBuffer);
    }

    if (!m_outputIODevice) {
        return;
    }
    constexpr int bytesPerSample{ AudioShared::getAudioFormat().bytesPerSample() };
    const auto samplesWritten{ m_bytesWritten / bytesPerSample };
    const qint64 numSamples{ bytesToWrite / bytesPerSample };

    if (m_monitorIODevice && m_monitorAudioDevice.enabled()) {

        // Synchronize devices if m_monitorAudioSink was just started
        if (m_monitorAudioSink->bytesFree() == m_monitorAudioSink->bufferSize()) {
            QByteArray padding{ m_outputAudioSink->bufferSize() - m_outputAudioSink->bytesFree(), '\0' };
            m_monitorIODevice->write(padding);
        }

        QByteArray samplesCopy{ m_currentBuffer.data<char>() + m_bytesWritten, bytesToWrite };
        if (m_monitorAudioDevice.overdrive()) {
            auto currentSamples{ reinterpret_cast<AudioShared::SampleType*>(samplesCopy.data()) };
            AudioShared::addOverdrive(currentSamples, numSamples, m_monitorAudioDevice.overdrive());
        }
        m_monitorIODevice->write(samplesCopy);
    }

    if (m_outputAudioDevice.overdrive()) {
        const auto currentSamples{ m_currentBuffer.data<AudioShared::SampleType>() + samplesWritten };
        AudioShared::addOverdrive(currentSamples, numSamples, m_outputAudioDevice.overdrive());
    }
    m_bytesWritten += m_outputIODevice->write(m_currentBuffer.data<char>() + m_bytesWritten, bytesToWrite);

    if (m_bytesWritten == m_currentBuffer.byteCount()) {
        m_bytesWritten = 0;
    }
    return QTimer::singleShot(1, this, &SoundEffect::processBuffer);
}

void SoundEffect::initAudioOutputSink()
{
    invalidateAudioOutputSink();
    if (m_outputAudioDevice.device().isNull()) {
        return;
    }

    constexpr QAudioFormat format{ AudioShared::getAudioFormat() };
    m_outputAudioSink = new QAudioSink{ m_outputAudioDevice.device(), format, this };
    m_outputAudioSink->setVolume(m_outputAudioDevice.volume());
    m_outputAudioSink->setBufferSize(
        format.sampleRate()
        * format.bytesPerSample()
        * format.channelCount()
        * 0.15 // seconds of buffering
    );

    connect(&m_outputAudioDevice, &AudioDevice::volumeChanged,
            m_outputAudioSink, &QAudioSink::setVolume);

    connect(m_outputAudioSink, &QAudioSink::stateChanged, this, [this](const QAudio::State state) {
        switch (state) {
        case QAudio::ActiveState:
            emit startedPlaying();
            break;
        case QAudio::SuspendedState:
        case QAudio::StoppedState:
        case QAudio::IdleState:
            m_currentBuffer = {};
            m_bytesWritten = 0;
            emit stoppedPlaying();
            break;
        default:
            break;
        }
    });
}

void SoundEffect::invalidateAudioOutputSink()
{
    if (!m_outputAudioSink) {
        return;
    }

    m_outputIODevice = nullptr;
    m_outputAudioSink->reset();
    delete m_outputAudioSink;
    m_outputAudioSink = nullptr;
}

void SoundEffect::initAudioMonitorSink()
{
    invalidateAudioMonitorSink();
    if (m_monitorAudioDevice.device().isNull()) {
        return;
    }

    constexpr QAudioFormat format{ AudioShared::getAudioFormat() };
    m_monitorAudioSink = new QAudioSink{ m_monitorAudioDevice.device(), format, this };
    m_monitorAudioSink->setVolume(m_monitorAudioDevice.volume());
    m_monitorAudioSink->setBufferSize(
        format.sampleRate()
        * format.bytesPerSample()
        * format.channelCount()
        * 0.20 // seconds of buffering
    );

    connect(&m_monitorAudioDevice, &AudioDevice::volumeChanged,
            m_monitorAudioSink, &QAudioSink::setVolume);
    if (m_monitorAudioDevice.enabled()) {
        m_monitorIODevice = m_monitorAudioSink->start();
    }
}

void SoundEffect::invalidateAudioMonitorSink()
{
    if (!m_monitorAudioSink) {
        return;
    }

    m_monitorIODevice = nullptr;
    m_monitorAudioSink->reset();
    delete m_monitorAudioSink;
    m_monitorAudioSink = nullptr;
}
