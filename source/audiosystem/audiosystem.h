#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include <QObject>
#include <QUrl>
#include <QMap>
#include <QAudioDevice>

class SoundEffect;

class AudioSystem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged FINAL)
    Q_PROPERTY(QAudioDevice outputDevice READ outputDevice WRITE setOutputDevice NOTIFY outputDeviceChanged FINAL)

public:
    explicit AudioSystem(QObject* const parent = nullptr);

    float volume() const;
    void setVolume(const float volume);

    Q_INVOKABLE void play(const int id, const QUrl& path);
    Q_INVOKABLE void stop(const int id) const;
    Q_INVOKABLE void stopAll() const;

    QAudioDevice outputDevice() const;
    void setOutputDevice(const QAudioDevice& outputDevice);

signals:
    void soundStarted(int id);
    void soundStopped(int id);
    void volumeChanged(float volume);
    void outputDeviceChanged(QAudioDevice audioDevice);

private:
    QMap<int, SoundEffect*> m_soundEffectMap{};
    QAudioDevice m_outputDevice{};

    float m_volume{ 1.f };
};

#endif // AUDIOSYSTEM_H
