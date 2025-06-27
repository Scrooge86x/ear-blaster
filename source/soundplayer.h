#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>

class SoundPlayer : public QObject
{
    Q_OBJECT
public:
    explicit SoundPlayer(QObject *parent = nullptr);

    // TODO: Implement setDevice and getDevices
    Q_INVOKABLE void play(const QString& filePath);
    Q_INVOKABLE void stop();
    Q_INVOKABLE void setVolume(const float volume);
    Q_INVOKABLE void setDevice(const QString& deviceId);
    Q_INVOKABLE QVariantList getDevices();
private:
    QMediaPlayer m_mediaPlayer{};
    QAudioOutput m_audioOutput{};
};

#endif // SOUNDPLAYER_H
