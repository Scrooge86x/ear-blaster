#include "microphonepassthrough.h"

#include "audiodevice.h"
#include "audioshared.h"

#include <QAudioDevice>
#include <QAudioSource>
#include <QAudioSink>
#include <QtTypes>
#include <QMediaDevices>

MicrophonePassthrough::MicrophonePassthrough()
    : QObject{ nullptr }
{
    m_inputAudioDevice = new AudioDevice{ this };
    connect(m_inputAudioDevice, &AudioDevice::deviceChanged,
            this, &MicrophonePassthrough::initAudioSource);

    m_outputAudioDevice = new AudioDevice{ this };
    connect(m_outputAudioDevice, &AudioDevice::deviceChanged,
            this, &MicrophonePassthrough::initAudioSink);

    connect(m_inputAudioDevice, &AudioDevice::enabledChanged, this, [this](const bool enabled) {
        if (enabled) {
            initAudioSink();
            initAudioSource();
        } else {
            invalidateAudioSource();
            invalidateAudioSink();
        }
    });

    const auto mediaDevices{ new QMediaDevices{ this } };
    connect(mediaDevices, &QMediaDevices::audioOutputsChanged, this, [this] {
        if (!QMediaDevices::audioOutputs().contains(m_outputAudioDevice->device())) {
            invalidateAudioSink();
        }
    });
    connect(mediaDevices, &QMediaDevices::audioInputsChanged, this, [this] {
        if (!QMediaDevices::audioInputs().contains(m_inputAudioDevice->device())) {
            invalidateAudioSource();
        }
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
    if (m_thread.isRunning()) {
        m_thread.quit();
        m_thread.wait();
    }
}

void MicrophonePassthrough::invalidateAudioSource()
{
    if (m_audioSource && m_inputIODevice) {
        m_inputIODevice = nullptr;
        delete m_audioSource;
        m_audioSource = nullptr;
    }
}

void MicrophonePassthrough::invalidateAudioSink()
{
    if (m_audioSink && m_outputIODevice) {
        m_outputIODevice = nullptr;
        m_audioSink->reset(); // Prevents IAudioClient3::GetCurrentPadding failed "AUDCLNT_E_DEVICE_INVALIDATED"
        delete m_audioSink;
        m_audioSink = nullptr;
    }
}

void MicrophonePassthrough::initAudioSink()
{
    invalidateAudioSink();
    if (m_outputAudioDevice->device().isNull() || !m_outputAudioDevice->enabled()) {
        return;
    }

    m_audioSink = new QAudioSink{ m_outputAudioDevice->device(), AudioShared::getAudioFormat(), this };
    m_audioSink->setVolume(m_outputAudioDevice->volume());
    connect(m_outputAudioDevice, &AudioDevice::volumeChanged,
            m_audioSink, &QAudioSink::setVolume);
    m_outputIODevice = m_audioSink->start();
}

void MicrophonePassthrough::initAudioSource()
{
    invalidateAudioSource();
    if (m_inputAudioDevice->device().isNull() || !m_inputAudioDevice->enabled()) {
        return;
    }

    m_audioSource = new QAudioSource{ m_inputAudioDevice->device(), AudioShared::getAudioFormat() };
    m_inputIODevice = m_audioSource->start();
    connect(m_inputIODevice, &QIODevice::readyRead,
            this, &MicrophonePassthrough::processBuffer);
}

void MicrophonePassthrough::processBuffer()
{
    if (!m_outputIODevice || !m_inputIODevice) {
        return;
    }

    QByteArray buffer{ m_inputIODevice->readAll() };
    AudioShared::addOverdrive(
        reinterpret_cast<AudioShared::SampleType*>(buffer.data()),
        buffer.length(),
        m_outputAudioDevice->overdrive()
    );
    m_outputIODevice->write(buffer);
}
