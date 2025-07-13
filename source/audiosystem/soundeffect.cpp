#include "soundeffect.h"

#include "audiodevice.h"
#include "audioshared.h"

#include <QAudioFormat>
#include <QAudioDecoder>
#include <QAudioSink>
#include <QTimer>

SoundEffect::SoundEffect(const AudioDevice& outputAudioDevice)
    : QObject{ nullptr }, m_outputAudioDevice{ outputAudioDevice }
{
    connect(&m_thread, &QThread::started, this, [this] {
        m_decoder = new QAudioDecoder{ this };
        m_decoder->setAudioFormat(AudioShared::getAudioFormat());

        connect(this, &SoundEffect::stopRequested,
                this, &SoundEffect::stop);
        connect(&m_outputAudioDevice, &AudioDevice::enabledChanged, this, [this](const bool enabled) {
            if (!enabled) {
                stop();
            }
        });

        connect(&m_outputAudioDevice, &AudioDevice::deviceChanged,
                this, &SoundEffect::initAudioSink);
        initAudioSink();
    });
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
        if (!m_audioSink || !m_outputAudioDevice.enabled()) {
            return;
        }

        // Reject last decoder buffer to prevent ASSERT: "!m_audioRenderer->isStepForced()"
        // that happens when using the ffmpeg backend
        if (m_decoder->bufferAvailable()) {
            static_cast<void>(m_decoder->read());
        }
        m_decoder->setSource(filePath);
        m_decoder->start();

        m_outputIODevice = m_audioSink->start();
        processBuffer();
    }, Qt::QueuedConnection);
}

void SoundEffect::stop()
{
    if (!m_audioSink || !m_outputIODevice) {
        return;
    }

    m_decoder->stop();
    m_audioSink->reset();
    m_audioSink->stop();
    m_outputIODevice = nullptr;
}

void SoundEffect::invalidateAudioSink()
{
    if (!m_audioSink) {
        return;
    }

    m_outputIODevice = nullptr;
    m_audioSink->reset();
    delete m_audioSink;
    m_audioSink = nullptr;
}

void SoundEffect::processBuffer()
{
    if (!m_bytesWritten) {
        if (!m_decoder->bufferAvailable()) {
            return QTimer::singleShot(1, this, &SoundEffect::processBuffer);
        }
        m_currentBuffer = m_decoder->read();
    }

    if (!m_audioSink) {
        return;
    }
    const qint64 bytesToWrite{ std::min(
        m_audioSink->bytesFree(), m_currentBuffer.byteCount() - m_bytesWritten
    ) };

    if (m_outputAudioDevice.overdrive()) {
        constexpr int bytesPerSample{ AudioShared::getAudioFormat().bytesPerSample() };
        const auto samplesWritten{ m_bytesWritten / bytesPerSample };

        const auto currentSamples{ m_currentBuffer.data<AudioShared::SampleType>() + samplesWritten };
        const qint64 numSamples{ bytesToWrite / bytesPerSample };
        AudioShared::addOverdrive(currentSamples, numSamples, m_outputAudioDevice.overdrive());
    }

    if (!m_audioSink->bytesFree()) {
        return QTimer::singleShot(50, this, &SoundEffect::processBuffer);
    }

    if (!m_outputIODevice) {
        return;
    }
    m_bytesWritten += m_outputIODevice->write(m_currentBuffer.data<char>() + m_bytesWritten, bytesToWrite);

    if (m_bytesWritten == m_currentBuffer.byteCount()) {
        m_bytesWritten = 0;
    }
    return QTimer::singleShot(1, this, &SoundEffect::processBuffer);
}

void SoundEffect::initAudioSink()
{
    invalidateAudioSink();
    if (m_outputAudioDevice.device().isNull()) {
        return;
    }

    constexpr QAudioFormat format{ AudioShared::getAudioFormat() };
    m_audioSink = new QAudioSink{ m_outputAudioDevice.device(), format, this };
    m_audioSink->setVolume(m_outputAudioDevice.volume());
    m_audioSink->setBufferSize(
        format.sampleRate()
        * format.bytesPerSample()
        * format.channelCount()
        * 0.15 // seconds of buffering
    );

    connect(&m_outputAudioDevice, &AudioDevice::volumeChanged,
            m_audioSink, &QAudioSink::setVolume);

    connect(m_audioSink, &QAudioSink::stateChanged, this, [this](const QAudio::State state) {
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
