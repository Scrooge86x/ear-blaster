#include "microphonepassthrough.h"

#include "audiodevice.h"
#include "audioshared.h"

#include <QAudioDevice>
#include <QAudioSource>
#include <QAudioSink>
#include <QtTypes>

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

void MicrophonePassthrough::invalidateInputDevice()
{
    if (!m_audioSource) {
        return;
    }

    m_inputDevice = nullptr;

    // If the source is not resumed before deletion it will block
    // the next audio source for some reason
    if (m_audioSource->state() == QtAudio::SuspendedState) {
        m_audioSource->resume();
    }
    delete m_audioSource;
    m_audioSource = nullptr;
}

void MicrophonePassthrough::invalidateOutputDevice()
{
    if (!m_audioSink) {
        return;
    }

    m_outputDevice = nullptr;
    m_audioSink->reset(); // Prevents IAudioClient3::GetCurrentPadding failed "AUDCLNT_E_DEVICE_INVALIDATED"
    delete m_audioSink;
    m_audioSink = nullptr;
}

void MicrophonePassthrough::start()
{
    QMetaObject::invokeMethod(this, [this]() {
        // Calling resume on audio source that is not suspended
        // causes IAudioClient3::Start failed "AUDCLNT_E_NOT_STOPPED"
        if (m_audioSource && m_audioSource->state() == QtAudio::SuspendedState) {
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
    invalidateOutputDevice();
    if (m_outputAudioDevice->device().isNull()) {
        return;
    }

    m_audioSink = new QAudioSink{ m_outputAudioDevice->device(), AudioShared::getAudioFormat(), this };
    m_audioSink->setVolume(m_outputAudioDevice->volume());
    connect(m_outputAudioDevice, &AudioDevice::volumeChanged,
            m_audioSink, &QAudioSink::setVolume);
    m_outputDevice = m_audioSink->start();

    // Reject last buffer so QIODevice::readyRead gets emmited again with new data
    if (m_inputDevice) {
        static_cast<void>(m_inputDevice->readAll());
    }
}

void MicrophonePassthrough::initAudioSource()
{
    invalidateInputDevice();
    if (m_inputAudioDevice->device().isNull()) {
        return;
    }

    m_audioSource = new QAudioSource{ m_inputAudioDevice->device(), AudioShared::getAudioFormat() };
    m_inputDevice = m_audioSource->start();
    if (!m_enabled) {
        m_audioSource->suspend();
    }
    connect(m_inputDevice, &QIODevice::readyRead,
            this, &MicrophonePassthrough::processBuffer);
}

void MicrophonePassthrough::processBuffer()
{
    if (!m_outputDevice || !m_inputDevice) {
        return;
    }
    QByteArray buffer{ m_inputDevice->readAll() };
    AudioShared::addOverdrive(
        reinterpret_cast<AudioShared::SampleType*>(buffer.data()),
        buffer.length(),
        m_outputAudioDevice->overdrive()
    );
    m_outputDevice->write(buffer);
}
