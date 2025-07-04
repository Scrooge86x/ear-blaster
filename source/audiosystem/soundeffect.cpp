#include "soundeffect.h"

#include <QAudioFormat>
#include <QAudioDecoder>
#include <QAudioSink>
#include <QTimer>

static consteval QAudioFormat getAudioFormat() {
    QAudioFormat format{};
    format.setChannelCount(2);
    format.setSampleFormat(QAudioFormat::Int16);
    format.setSampleRate(48'000);
    return format;
}

SoundEffect::SoundEffect(QObject* const parent)
    : QObject{ parent }
{
    m_decoder = new QAudioDecoder{ this };
    m_decoder->setAudioFormat(getAudioFormat());
}

void SoundEffect::play(const QUrl& filePath)
{
    m_decoder->setSource(filePath);
    m_decoder->start();
    processBuffer();
}

void SoundEffect::stop() const
{
    m_decoder->stop();
    m_audioSink->stop();
}

float SoundEffect::volume() const
{
    return m_volume;
}

void SoundEffect::setVolume(const float volume)
{
    m_volume = volume;
}

void SoundEffect::setVolume(const float* const volumePtr)
{
    m_volumePtr = volumePtr;
}

QAudioDevice SoundEffect::outputDevice() const
{
    return m_outputDevice;
}

void SoundEffect::setOutputDevice(const QAudioDevice& outputDevice)
{
    if (m_outputDevice == outputDevice) {
        return;
    }
    m_outputDevice = outputDevice;

    if (m_audioSink) {
        m_audioSink->stop();
        delete m_audioSink;
    }

    constexpr QAudioFormat format{ getAudioFormat() };
    m_audioSink = new QAudioSink{ m_outputDevice, format, this };
    m_audioSink->setBufferSize(
        format.sampleRate()
        * format.bytesPerSample()
        * format.channelCount()
        * 2 // seconds of buffering
    );
    m_ioDevice = m_audioSink->start();

    connect(m_audioSink, &QAudioSink::stateChanged, this, [this](const QAudio::State state){
        switch (state) {
        case QAudio::ActiveState:
            emit startedPlaying();
            break;
        case QAudio::SuspendedState:
        case QAudio::StoppedState:
        case QAudio::IdleState:
            emit stoppedPlaying();
            break;
        default:
            break;
        }
    });
}

void SoundEffect::processBuffer()
{
    if (!m_bytesWritten) {
        if (!m_decoder->bufferAvailable()) {
            return QTimer::singleShot(1, this, [this]{ processBuffer(); });
        }
        m_currentBuffer = m_decoder->read();

        const float volume{ m_volumePtr ? *m_volumePtr : m_volume };
        const auto data{ m_currentBuffer.data<char>() };
        for (int i{}; i < m_currentBuffer.byteCount(); ++i) {
            data[i] *= volume;
        }
    }

    const auto bytesLeft{ m_currentBuffer.byteCount() - m_bytesWritten };
    if (!m_audioSink->bytesFree()) {
        return QTimer::singleShot(1000, this, [this]{ processBuffer(); });
    }

    const auto offsetData{ m_currentBuffer.data<char>() + m_bytesWritten };
    m_bytesWritten += m_ioDevice->write(offsetData, std::clamp(m_audioSink->bytesFree(), 0ll, bytesLeft));
    if (m_audioSink->state() != QAudio::ActiveState) {
        return;
    }

    if (m_bytesWritten == m_currentBuffer.byteCount()) {
        m_bytesWritten = 0;
    }
    return QTimer::singleShot(1, this, [this]{ processBuffer(); });
}
