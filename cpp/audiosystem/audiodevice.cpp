#include "audiodevice.h"

#include <QtNumeric>

void AudioDevice::setDevice(const QAudioDevice& device)
{
    if (m_device != device) {
        m_device = device;
        emit deviceChanged();
    }
}

void AudioDevice::setVolume(const float volume)
{
    if (!qFuzzyCompare(m_volume, volume)) {
        m_volume = volume;
        emit volumeChanged(volume);
    }
}

void AudioDevice::setOverdrive(const float overdrive)
{
    if (!qFuzzyCompare(m_overdrive, overdrive)) {
        m_overdrive = overdrive;
        emit overdriveChanged(overdrive);
    }
}

void AudioDevice::setEnabled(const bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        emit enabledChanged(m_enabled);
    }
}
