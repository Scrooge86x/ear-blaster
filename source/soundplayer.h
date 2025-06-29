#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QAudioDevice>

class SoundPlayer : public QObject
{
    Q_OBJECT
public:
    explicit SoundPlayer(QObject* parent = nullptr);

    Q_INVOKABLE void play(const int id, const QString& filePath);
    Q_INVOKABLE void stop(const int id);
    Q_INVOKABLE void setVolume(const float volume);
    Q_INVOKABLE void setDevice(const QAudioDevice& device);
    Q_INVOKABLE QAudioDevice getDevice() const;
private:
    QMediaPlayer m_mediaPlayer{};
    QAudioOutput m_audioOutput{};
    int m_currentTrackId{};
};

#endif // SOUNDPLAYER_H
