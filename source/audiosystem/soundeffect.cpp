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

template <QAudioFormat::SampleFormat> struct SampleFormatType { using type = void; };
template <> struct SampleFormatType<QAudioFormat::UInt8> { using type = quint8; };
template <> struct SampleFormatType<QAudioFormat::Int16> { using type = qint16; };
template <> struct SampleFormatType<QAudioFormat::Int32> { using type = qint32; };
template <> struct SampleFormatType<QAudioFormat::Float> { using type = float; };

using SampleType = SampleFormatType<getAudioFormat().sampleFormat()>::type;

SoundEffect::SoundEffect()
    : QObject{ nullptr }
{
    connect(&m_thread, &QThread::started, this, [this] {
        m_decoder = new QAudioDecoder{ this };
        m_decoder->setAudioFormat(getAudioFormat());
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

    QMetaObject::invokeMethod(this, [this]() {
        if (m_audioSink) {
            m_audioSink->reset();
            m_audioSink->stop();
            m_ioDevice = nullptr;
            delete m_audioSink;
        }

        constexpr QAudioFormat format{ getAudioFormat() };
        m_audioSink = new QAudioSink{ m_outputDevice, format, this };
        m_audioSink->setBufferSize(
            format.sampleRate()
            * format.bytesPerSample()
            * format.channelCount()
            * 0.15 // seconds of buffering
        );

        connect(m_audioSink, &QAudioSink::stateChanged, this, [this](const QAudio::State state){
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
    }, Qt::QueuedConnection);
}

void SoundEffect::processBuffer()
{
    if (!m_bytesWritten) {
        if (!m_decoder->bufferAvailable()) {
            return QTimer::singleShot(1, this, &SoundEffect::processBuffer);
        }
        m_currentBuffer = m_decoder->read();
    }

    const float volume{ m_volumePtr ? *m_volumePtr : m_volume };

    constexpr int bytesPerSample{ getAudioFormat().bytesPerSample() };
    const auto samplesWritten{ m_bytesWritten / bytesPerSample };
    const auto sampleData{ m_currentBuffer.data<SampleType>() + samplesWritten };

    const qint64 bytesToWrite{ std::min(m_audioSink->bytesFree(), m_currentBuffer.byteCount() - m_bytesWritten) };
    const qint64 numSamples{ bytesToWrite / bytesPerSample };
    for (qint64 i{}; i < numSamples; ++i) {
        sampleData[i] *= volume;
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
