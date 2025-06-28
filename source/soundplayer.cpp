#include "soundplayer.h"

#include <QAudioDevice>
#include <QMediaDevices>
#include <QtMinMax>
#include <QVariant>

SoundPlayer::SoundPlayer(QObject* parent)
    : QObject{ parent }
{
    m_audioOutput.setDevice(QMediaDevices::defaultAudioOutput());
    m_audioOutput.setVolume(1.f);
    m_mediaPlayer.setAudioOutput(&m_audioOutput);
}

void SoundPlayer::play(const int id, const QString& filePath)
{
    m_currentTrackId = id;
    m_mediaPlayer.setSource(QUrl::fromLocalFile(filePath));
    m_mediaPlayer.play();
}

void SoundPlayer::stop(const int id)
{
    if (m_currentTrackId == id) {
        m_mediaPlayer.stop();
    }
}

void SoundPlayer::setVolume(const float volume)
{
    m_audioOutput.setVolume(volume);
}

void SoundPlayer::setDevice(const QString& deviceId)
{
    for (const QAudioDevice& device : QMediaDevices::audioOutputs()) {
        if (device.id() == deviceId) {
            m_audioOutput.setDevice(device);
            return;
        }
    }
    qWarning() << "Device not found:" << deviceId;
}

QVariantList SoundPlayer::getDevices()
{
    QVariantList devices{};
    for (const QAudioDevice& device : QMediaDevices::audioOutputs()) {
        devices.append(QVariantMap{
            { "name", device.description() },
            { "id", device.id() },
        });
    }
    return devices;
}
