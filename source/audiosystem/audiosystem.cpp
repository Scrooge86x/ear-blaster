#include "audiosystem.h"

#include "soundeffect.h"
#include "audiodevice.h"
#include "microphonepassthrough.h"

#include <QMediaDevices>

AudioSystem::AudioSystem(QObject *const parent)
    : QObject{ parent }
{
    m_outputDevice   = new AudioDevice{ this };
    m_micPassthrough = new MicrophonePassthrough{};
    connect(m_outputDevice, &AudioDevice::deviceChanged, this, [this] {
        m_micPassthrough->outputDevice()->setDevice(m_outputDevice->device());
    });

    const auto mediaDevices{ new QMediaDevices{ this } };
    connect(mediaDevices, &QMediaDevices::audioOutputsChanged, this, [this] {
        if (!QMediaDevices::audioOutputs().contains(m_outputDevice->device())) {
            stopAll();
            m_micPassthrough->invalidateOutputDevice();
        }
        emit audioOutputsChanged();
    });
    connect(mediaDevices, &QMediaDevices::audioInputsChanged, this, [this] {
        if (!QMediaDevices::audioInputs().contains(m_micPassthrough->inputDevice()->device())) {
            m_micPassthrough->invalidateInputDevice();
        }
        emit audioInputsChanged();
    });
}

AudioSystem::~AudioSystem()
{
    for (const auto soundEffect : std::as_const(m_soundEffectMap)) {
        delete soundEffect;
    }
    delete m_micPassthrough;
}

void AudioSystem::play(const int id, const QUrl& path)
{
    if (m_soundEffectMap.contains(id)) {
        m_soundEffectMap[id]->stop();
        m_soundEffectMap[id]->play(path);
        return;
    }

    auto& soundEffect{ m_soundEffectMap[id] };
    soundEffect = new SoundEffect{};
    connect(soundEffect, &SoundEffect::startedPlaying, this, [this, id]{ emit soundStarted(id); });
    connect(soundEffect, &SoundEffect::stoppedPlaying, this, [this, id]{ emit soundStopped(id); });
    soundEffect->setOutputDevice(m_outputDevice);
    soundEffect->play(path);
}

void AudioSystem::stop(const int id) const
{
    if (m_soundEffectMap.contains(id)) {
        m_soundEffectMap[id]->stop();
    }
}

void AudioSystem::stopAll() const
{
    for (const auto soundEffect : std::as_const(m_soundEffectMap)) {
        soundEffect->stop();
    }
}

QList<QAudioDevice> AudioSystem::audioInputs() const
{
    auto audioInputs{ QMediaDevices::audioInputs() };
    audioInputs.push_front({});
    return audioInputs;
}

QList<QAudioDevice> AudioSystem::audioOutputs() const
{
    auto audioOutputs{ QMediaDevices::audioOutputs() };
    audioOutputs.push_front({});
    return audioOutputs;
}

QAudioDevice AudioSystem::getInputDeviceById(const QString& id)
{
    const auto inputDevices{ QMediaDevices::audioInputs() };
    for (const auto& device : inputDevices) {
        if (device.id() == id) {
            return device;
        }
    }
    return {};
}

QAudioDevice AudioSystem::getOutputDeviceById(const QString& id)
{
    const auto outputDevices{ QMediaDevices::audioOutputs() };
    for (const auto& device : outputDevices) {
        if (device.id() == id) {
            return device;
        }
    }
    return {};
}

qsizetype AudioSystem::getInputDeviceIndexById(const QString& id)
{
    const auto inputDevices{ QMediaDevices::audioInputs() };
    const auto numDevices{ inputDevices.length() };
    for (qsizetype i{}; i < numDevices; ++i) {
        if (inputDevices[i].id() == id) {
            return i + 1;
        }
    }
    return 0;
}

qsizetype AudioSystem::getOutputDeviceIndexById(const QString& id)
{
    const auto outputDevices{ QMediaDevices::audioOutputs() };
    const auto numDevices{ outputDevices.length() };
    for (qsizetype i{}; i < numDevices; ++i) {
        if (outputDevices[i].id() == id) {
            return i + 1;
        }
    }
    return 0;
}
