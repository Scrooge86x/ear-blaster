#include "microphonepassthrough.h"

#include "audiodevice.h"

#include <QAudioDevice>
#include <QAudioSource>
#include <QAudioSink>

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

MicrophonePassthrough::MicrophonePassthrough()
    : QObject{ nullptr }
{
    connect(&m_thread, &QThread::started, this, [this] {
        m_inputAudioDevice = new AudioDevice{ this };
        connect(m_inputAudioDevice, &AudioDevice::deviceChanged,
                this, &MicrophonePassthrough::initAudioSource);

        m_outputAudioDevice = new AudioDevice{ this };
        connect(m_outputAudioDevice, &AudioDevice::deviceChanged,
                this, &MicrophonePassthrough::initAudioSink);
    });
    m_thread.start();
    this->moveToThread(&m_thread);
}

MicrophonePassthrough::~MicrophonePassthrough()
{
    // Workaround for "QWinEventNotifier: Event notifiers cannot be enabled or disabled from another thread"
    if (m_audioSource) {
        QMetaObject::invokeMethod(m_audioSource, &QObject::deleteLater, Qt::BlockingQueuedConnection);
    }
    m_thread.quit();
    m_thread.wait();
}

void MicrophonePassthrough::setEnabled(const bool enabled)
{
    if (m_enabled == enabled) {
        return;
    }
    m_enabled = enabled;
    m_enabled ? start() : stop();
    emit enabledChanged(m_enabled);
}

void MicrophonePassthrough::start()
{
    QMetaObject::invokeMethod(this, [this]() {
        if (m_audioSource) {
            m_audioSource->resume();
        }
    }, Qt::QueuedConnection);
}

void MicrophonePassthrough::stop()
{
    QMetaObject::invokeMethod(this, [this]() {
        if (m_audioSource) {
            m_audioSource->suspend();
        }
    }, Qt::QueuedConnection);
}

void MicrophonePassthrough::initAudioSink()
{
    QAudioSink* oldAudioSink{ m_audioSink };

    m_audioSink = new QAudioSink{ m_outputAudioDevice->device(), getAudioFormat(), this };
    m_audioSink->setVolume(m_outputAudioDevice->volume());
    connect(m_outputAudioDevice, &AudioDevice::volumeChanged,
            m_audioSink, &QAudioSink::setVolume);
    m_outputDevice = m_audioSink->start();

    if (oldAudioSink) {
        delete oldAudioSink;
    }
}

void MicrophonePassthrough::initAudioSource()
{
    QAudioSource* oldAudioSource{ m_audioSource };
    m_audioSource = new QAudioSource{ m_inputAudioDevice->device(), getAudioFormat() };
    m_inputDevice = m_audioSource->start();
    if (!m_enabled) {
        m_audioSource->suspend();
    }
    connect(m_inputDevice, &QIODevice::readyRead,
            this, &MicrophonePassthrough::processBuffer);

    // This also deletes m_inputDevice so no need to disconnect manually
    if (oldAudioSource) {
        delete oldAudioSource;
    }
}

void MicrophonePassthrough::processBuffer()
{
    if (!m_outputDevice || !m_inputDevice) {
        return;
    }
    m_outputDevice->write(m_inputDevice->readAll());
}
