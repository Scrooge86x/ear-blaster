#include "soundplayer.h"

#include <QAudioDevice>
#include <QMediaDevices>
#include <QtMinMax>

SoundPlayer::SoundPlayer(QObject *parent)
    : QObject{ parent }
{
    m_audioOutput.setDevice(QMediaDevices::defaultAudioOutput());
    m_audioOutput.setVolume(1.f);
    m_mediaPlayer.setAudioOutput(&m_audioOutput);
}

void SoundPlayer::play(const QString &filePath)
{
    m_mediaPlayer.setSource(QUrl::fromLocalFile(filePath));
    m_mediaPlayer.play();
}

void SoundPlayer::stop()
{
    m_mediaPlayer.stop();
}

void SoundPlayer::setVolume(const float volume)
{
    m_audioOutput.setVolume(qBound(0.f, volume, 1.f));
}
