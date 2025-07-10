#include "audiosystem.h"

#include "soundeffect.h"
#include "audiodevice.h"
#include "microphonepassthrough.h"

AudioSystem::AudioSystem(QObject *const parent)
    : QObject{ parent }
{
    m_outputDevice = new AudioDevice{ this };
    m_inputDevice  = new AudioDevice{ this };

    m_micPassthrough = new MicrophonePassthrough{};
    m_micPassthrough->setOutputDevice(m_outputDevice);
    m_micPassthrough->setInputDevice(m_inputDevice);
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
