#include "soundeffect.h"

#include "audiodevice.h"
#include "audioshared.h"

#include <QAudioFormat>
#include <QAudioDecoder>
#include <QAudioSink>
#include <QTimer>

SoundEffect::SoundEffect()
    : QObject{ nullptr }
{
    connect(&m_thread, &QThread::started, this, [this] {
        m_decoder = new QAudioDecoder{ this };
        m_decoder->setAudioFormat(AudioShared::getAudioFormat());
    });
    m_thread.start();
    this->moveToThread(&m_thread);
}

SoundEffect::~SoundEffect()
{
    m_thread.quit();
    m_thread.wait();
}

void SoundEffect::play(const QUrl& filePath)
{
    QMetaObject::invokeMethod(this, [this, filePath]() {
        if (!m_audioSink) {
            return;
        }

        m_ioDevice = m_audioSink->start();
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
    QMetaObject::invokeMethod(this, [this]() {
        if (!m_audioSink) {
            return;
        }

        m_decoder->stop();
        m_audioSink->reset();
        m_audioSink->stop();
        m_ioDevice = nullptr;
    }, Qt::QueuedConnection);
}

void SoundEffect::setOutputDevice(const AudioDevice* const outputDevice)
{
    if (!outputDevice || m_outputDevice == outputDevice) {
        return;
    }
    if (m_outputDevice) {
        disconnect(m_outputDevice, &AudioDevice::volumeChanged, this, nullptr);
    }

    m_outputDevice = outputDevice;
    connect(m_outputDevice, &AudioDevice::volumeChanged, this, [this] {
        if (m_audioSink) {
            m_audioSink->setVolume(m_outputDevice->volume());
        }
    });

    const auto updateAudioSink{ [this]() {
        if (m_audioSink) {
            m_audioSink->reset();
            m_audioSink->stop();
            m_ioDevice = nullptr;
            delete m_audioSink;
            m_audioSink = nullptr;
        }

        if (m_outputDevice->device().isNull()) {
            return;
        }

        constexpr QAudioFormat format{ AudioShared::getAudioFormat() };
        m_audioSink = new QAudioSink{ m_outputDevice->device(), format, this };
        m_audioSink->setVolume(m_outputDevice->volume());
        m_audioSink->setBufferSize(
            format.sampleRate()
            * format.bytesPerSample()
            * format.channelCount()
            * 0.15 // seconds of buffering
        );

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
    } };

    connect(m_outputDevice, &AudioDevice::deviceChanged, this, updateAudioSink);
    QMetaObject::invokeMethod(this, updateAudioSink, Qt::QueuedConnection);
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

    if (m_outputDevice->overdrive()) {
        constexpr int bytesPerSample{ AudioShared::getAudioFormat().bytesPerSample() };
        const auto samplesWritten{ m_bytesWritten / bytesPerSample };

        const auto currentSamples{ m_currentBuffer.data<AudioShared::SampleType>() + samplesWritten };
        const qint64 numSamples{ bytesToWrite / bytesPerSample };
        AudioShared::addOverdrive(currentSamples, numSamples, m_outputDevice->overdrive());
    }

    if (!m_audioSink->bytesFree()) {
        return QTimer::singleShot(50, this, &SoundEffect::processBuffer);
    }

    if (!m_ioDevice) {
        return;
    }
    m_bytesWritten += m_ioDevice->write(m_currentBuffer.data<char>() + m_bytesWritten, bytesToWrite);

    if (m_bytesWritten == m_currentBuffer.byteCount()) {
        m_bytesWritten = 0;
    }
    return QTimer::singleShot(1, this, &SoundEffect::processBuffer);
}
