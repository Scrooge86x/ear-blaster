#include "soundplayer.h"

#include <QMediaDevices>

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

void SoundPlayer::setDevice(const QAudioDevice& device)
{
    m_audioOutput.setDevice(device);
}

QAudioDevice SoundPlayer::getDevice() const
{
    return m_audioOutput.device();
}
