#include "microphonepassthrough.h"

#include "audiodevice.h"
#include "audioshared.h"

#include <QAudioDevice>
#include <QAudioSource>
#include <QAudioSink>
#include <QtTypes>
#include <QMediaDevices>

MicrophonePassthrough::MicrophonePassthrough(const AudioDevice& outputAudioDevice)
    : QObject{ nullptr }
    , m_audioOutput{ outputAudioDevice, { .initializeVolume{ false } }, this }
    , m_outputAudioDevice{ outputAudioDevice }
{
    m_inputAudioDevice = new AudioDevice{ this };
    m_inputAudioDevice->setEnabled(false);
    connect(m_inputAudioDevice, &AudioDevice::deviceChanged,
            this, &MicrophonePassthrough::initAudioSource);

    connect(m_inputAudioDevice, &AudioDevice::enabledChanged, this, [this](const bool enabled) {
        if (enabled) {
            m_audioOutput.start();
            initAudioSource();
        } else {
            invalidateAudioSource();
            m_audioOutput.stop();
        }
    });

    const auto mediaDevices{ new QMediaDevices{ this } };
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
    if (!m_audioSource || !m_inputIODevice) {
        return;
    }

    m_inputIODevice = nullptr;
    delete m_audioSource;
    m_audioSource = nullptr;
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
    QIODevice* const outputIODevice{ m_audioOutput.ioDevice() };
    if (!outputIODevice || !m_inputIODevice) {
        return;
    }

    QByteArray buffer{ m_inputIODevice->readAll() };
    AudioShared::addOverdrive(
        reinterpret_cast<AudioShared::SampleType*>(buffer.data()),
        buffer.length(),
        m_inputAudioDevice->overdrive()
    );
    outputIODevice->write(buffer);
}
