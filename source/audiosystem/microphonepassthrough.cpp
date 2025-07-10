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

void MicrophonePassthrough::setInputDevice(const AudioDevice* const inputAudioDevice)
{
    if (!inputAudioDevice || m_inputAudioDevice == inputAudioDevice) {
        return;
    }
    if (m_inputAudioDevice) {
        disconnect(m_inputAudioDevice, &AudioDevice::deviceChanged, this, nullptr);
    }

    m_inputAudioDevice = inputAudioDevice;

    connect(m_inputAudioDevice, &AudioDevice::deviceChanged, this, [this]() {
        if (m_audioSource) {
            m_audioSource->reset();
            m_audioSource->stop();
            m_inputDevice = nullptr;
            delete m_audioSource;
        }

        m_audioSource = new QAudioSource{ m_inputAudioDevice->device(), getAudioFormat() };
        if (!m_inputDevice && m_enabled) {
            start();
        }
    });
}

void MicrophonePassthrough::setOutputDevice(const AudioDevice* const outputAudioDevice)
{
    if (!outputAudioDevice || m_outputAudioDevice == outputAudioDevice) {
        return;
    }
    if (m_outputAudioDevice) {
        disconnect(m_outputAudioDevice, &AudioDevice::volumeChanged, this, nullptr);
        disconnect(m_outputAudioDevice, &AudioDevice::deviceChanged, this, nullptr);
    }

    m_outputAudioDevice = outputAudioDevice;
    connect(m_outputAudioDevice, &AudioDevice::volumeChanged, this, [this] {
        if (m_audioSink) {
            m_audioSink->setVolume(m_outputAudioDevice->volume());
        }
    });

    connect(m_outputAudioDevice, &AudioDevice::deviceChanged, this, [this]() {
        if (m_audioSink) {
            m_audioSink->reset();
            m_audioSink->stop();
            m_outputDevice = nullptr;
            delete m_audioSink;
        }

        m_audioSink = new QAudioSink{ m_outputAudioDevice->device(), getAudioFormat(), this };
        m_audioSink->setVolume(m_outputAudioDevice->volume());
        if (!m_outputDevice && m_enabled) {
            start();
        }
    });
}

void MicrophonePassthrough::start()
{
    QMetaObject::invokeMethod(this, [this]() {
        if (!m_audioSource || !m_audioSink) {
            return;
        }

        if (!m_inputDevice) {
            m_inputDevice = m_audioSource->start();
            connect(m_inputDevice, &QIODevice::readyRead, this, &MicrophonePassthrough::processBuffer);
        }
        if (!m_outputDevice) {
            m_outputDevice = m_audioSink->start();
        }
    }, Qt::QueuedConnection);
}

void MicrophonePassthrough::stop()
{
    QMetaObject::invokeMethod(this, [this]() {
        if (m_audioSource) {
            m_audioSource->reset();
            m_audioSource->stop();
            m_inputDevice = nullptr;
        }
        if (m_audioSink) {
            m_audioSink->reset();
            m_audioSink->stop();
            m_outputDevice = nullptr;
        }
    }, Qt::QueuedConnection);
}

void MicrophonePassthrough::processBuffer()
{
    if (!m_outputDevice || !m_inputDevice) {
        return;
    }
    m_outputDevice->write(m_inputDevice->readAll());
}
