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
    , m_audioOutput{ outputAudioDevice, {}, this }
    , m_monitorOutput{ monitorAudioDevice, {}, this }
{
    connect(this, &SoundEffect::stopRequested,
            this, &SoundEffect::stop);

    connect(&m_audioOutput, &AudioOutput::initialized,
            this, &SoundEffect::onAudioOutputInit);

    connect(&m_monitorAudioDevice, &AudioDevice::enabledChanged, this, [this](const bool enabled) {
        if (m_audioOutput.ioDevice()) {
            m_monitorOutput.start();
        }
    });

    connect(&m_thread, &QThread::started, this, [this] {
        m_decoder = new QAudioDecoder{ this };
        m_decoder->setAudioFormat(AudioShared::getAudioFormat());

        m_audioOutput.initialize();
        m_monitorOutput.initialize();
    });

    this->moveToThread(&m_thread);
    m_thread.start();
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
        if (!m_audioOutput.start()) {
            return;
        }
        m_monitorOutput.start();

        // Reject last decoder buffer to prevent ASSERT: "!m_audioRenderer->isStepForced()"
        // that happens when using the ffmpeg backend
        if (m_decoder->bufferAvailable()) {
            static_cast<void>(m_decoder->read());
        }
        m_decoder->setSource(filePath);
        m_decoder->start();

        processBuffer();
    }, Qt::QueuedConnection);
}

void SoundEffect::stop()
{
    if (m_decoder->isDecoding()) {
        m_decoder->stop();
    }

    m_audioOutput.stop();
    m_monitorOutput.stop();
}

void SoundEffect::onAudioOutputInit()
{
    const QAudioSink* const outputAudioSink{ m_audioOutput.audioSink() };
    connect(outputAudioSink, &QAudioSink::stateChanged,
            this, [this, previousState = QAudio::StoppedState](const QAudio::State currentState) mutable {
        switch (currentState) {
        case QAudio::ActiveState:
            emit startedPlaying();
            break;
        case QAudio::SuspendedState:
        case QAudio::StoppedState:
        case QAudio::IdleState:
            if (previousState != QAudio::ActiveState) {
                break;
            }

            m_currentBuffer = {};
            m_bytesWritten = 0;
            emit stoppedPlaying();
            break;
        default:
            break;
        }

        previousState = currentState;
    });
}

void SoundEffect::processBuffer()
{
    if (!m_bytesWritten) {
        if (!m_decoder->bufferAvailable()) {
            return QTimer::singleShot(1, this, &SoundEffect::processBuffer);
        }
        m_currentBuffer = m_decoder->read();
    }

    // If this check passes audioSink will NEVER be a nullptr
    QIODevice* const outputIODevice{ m_audioOutput.ioDevice() };
    if (!outputIODevice) {
        return;
    }

    const QAudioSink* const outputAudioSink{ m_audioOutput.audioSink() };
    if (!outputAudioSink->bytesFree()) {
        return QTimer::singleShot(50, this, &SoundEffect::processBuffer);
    }

    constexpr int bytesPerSample{ AudioShared::getAudioFormat().bytesPerSample() };
    const qint64 bytesToWrite{ std::min(
        outputAudioSink->bytesFree(), m_currentBuffer.byteCount() - m_bytesWritten
    ) };
    const qint64 numSamples{ bytesToWrite / bytesPerSample };

    QIODevice* const monitorIODevice{ m_monitorOutput.ioDevice() };
    if (monitorIODevice) {
        const QAudioSink* const monitorAudioSink{ m_monitorOutput.audioSink() };

        // Synchronize devices if monitorAudioSink was just started
        if (monitorAudioSink->bytesFree() == monitorAudioSink->bufferSize()) {
            QByteArray padding{ outputAudioSink->bufferSize() - outputAudioSink->bytesFree(), '\0' };
            monitorIODevice->write(padding);
        }

        QByteArray samplesCopy{ m_currentBuffer.data<char>() + m_bytesWritten, bytesToWrite };
        if (m_monitorAudioDevice.overdrive()) {
            AudioShared::addOverdrive(
                samplesCopy.data(),
                AudioShared::getAudioFormat().sampleFormat(),
                numSamples,
                m_monitorAudioDevice.overdrive()
            );
        }
        monitorIODevice->write(samplesCopy);
    }

    if (m_outputAudioDevice.overdrive()) {
        AudioShared::addOverdrive(
            m_currentBuffer.data<char>() + m_bytesWritten,
            AudioShared::getAudioFormat().sampleFormat(),
            numSamples,
            m_outputAudioDevice.overdrive()
        );
    }
    m_bytesWritten += outputIODevice->write(m_currentBuffer.data<char>() + m_bytesWritten, bytesToWrite);

    if (m_bytesWritten == m_currentBuffer.byteCount()) {
        m_bytesWritten = 0;
    }
    return QTimer::singleShot(1, this, &SoundEffect::processBuffer);
}
