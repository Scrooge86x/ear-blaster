#include "audiosystem.h"

#include "soundeffect.h"
#include "audiodevice.h"
#include "microphonepassthrough.h"
#include "texttospeech.h"

#include <QMediaDevices>

AudioSystem::AudioSystem(QObject *const parent)
    : QObject{ parent }
{
    m_outputAudioDevice  = new AudioDevice{ this };
    m_monitorAudioDevice = new AudioDevice{ this };

    m_micPassthrough = std::make_unique<MicrophonePassthrough>(*m_outputAudioDevice);
    m_outputAudioDevice->setEnabled(true);

    m_tts = std::make_unique<TextToSpeech>(*m_outputAudioDevice, *m_monitorAudioDevice);

    const auto mediaDevices{ new QMediaDevices{ this } };
    connect(mediaDevices, &QMediaDevices::audioOutputsChanged,
            this, &AudioSystem::audioOutputsChanged);
    connect(mediaDevices, &QMediaDevices::audioInputsChanged,
            this, &AudioSystem::audioInputsChanged);
}

AudioSystem::~AudioSystem()
{
    for (const auto& soundEffect : std::as_const(m_soundEffectMap)) {
        soundEffect->deleteLater();
    }
    m_soundEffectMap.clear();
}

void AudioSystem::play(const int id, const QUrl& path)
{
    if (m_soundEffectMap.contains(id)) {
        emit m_soundEffectMap[id]->stopRequested();
        m_soundEffectMap[id]->play(path);
        return;
    }

    auto& soundEffect = m_soundEffectMap[id];
    soundEffect = new SoundEffect{ *m_outputAudioDevice, *m_monitorAudioDevice };
    connect(soundEffect, &SoundEffect::startedPlaying, this, [this, id]{ emit soundStarted(id); });
    connect(soundEffect, &SoundEffect::stoppedPlaying, this, [this, id]{ emit soundStopped(id); });
    soundEffect->play(path);
}

void AudioSystem::stop(const int id) const
{
    if (m_soundEffectMap.contains(id)) {
        emit m_soundEffectMap[id]->stopRequested();
    }
}

void AudioSystem::stopAll() const
{
    for (const auto& soundEffect : std::as_const(m_soundEffectMap)) {
        emit soundEffect->stopRequested();
    }
}

QList<QAudioDevice> AudioSystem::audioInputs()
{
    auto audioInputs{ QMediaDevices::audioInputs() };
    audioInputs.push_front({});
    return audioInputs;
}

QList<QAudioDevice> AudioSystem::audioOutputs()
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
