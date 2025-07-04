#include "audiosystem.h"

#include "soundeffect.h"

AudioSystem::AudioSystem(QObject *const parent)
    : QObject{ parent }
{}

float AudioSystem::volume() const
{
    return m_volume;
}

void AudioSystem::setVolume(const float volume)
{
    if (volume != m_volume) {
        m_volume = volume;
        emit volumeChanged(volume);
    }
}

void AudioSystem::play(const int id, const QUrl& path)
{
    if (m_soundEffectMap.contains(id)) {
        m_soundEffectMap[id]->stop();
        delete m_soundEffectMap[id];
    }

    auto& soundEffect{ m_soundEffectMap[id] };
    soundEffect = new SoundEffect{ this };
    connect(soundEffect, &SoundEffect::startedPlaying, this, [this, id]{ emit soundStarted(id); });
    connect(soundEffect, &SoundEffect::stoppedPlaying, this, [this, id]{ emit soundStopped(id); });
    soundEffect->setOutputDevice(m_outputDevice);
    soundEffect->setVolume(&m_volume);
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
    for (const auto* const soundEffect : std::as_const(m_soundEffectMap)) {
        soundEffect->stop();
    }
}

QAudioDevice AudioSystem::outputDevice() const
{
    return m_outputDevice;
}

void AudioSystem::setOutputDevice(const QAudioDevice& outputDevice)
{
    if (m_outputDevice == outputDevice) {
        return;
    }

    m_outputDevice = outputDevice;
    for (const auto soundEffect : std::as_const(m_soundEffectMap)) {
        soundEffect->setOutputDevice(m_outputDevice);
    }
    emit outputDeviceChanged(m_outputDevice);
}
