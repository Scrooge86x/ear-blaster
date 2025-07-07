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
    Q_PROPERTY(float overdrive READ overdrive WRITE setOverdrive NOTIFY overdriveChanged FINAL)
    Q_PROPERTY(QAudioDevice outputDevice READ outputDevice WRITE setOutputDevice NOTIFY outputDeviceChanged FINAL)

public:
    explicit AudioSystem(QObject* const parent = nullptr);
    ~AudioSystem();

    float volume() const;
    void setVolume(const float volume);

    float overdrive() const;
    void setOverdrive(const float overdrive);

    Q_INVOKABLE void play(const int id, const QUrl& path);
    Q_INVOKABLE void stop(const int id) const;
    Q_INVOKABLE void stopAll() const;

    QAudioDevice outputDevice() const;
    void setOutputDevice(const QAudioDevice& outputDevice);

signals:
    void soundStarted(int id);
    void soundStopped(int id);
    void volumeChanged(float volume);
    void overdriveChanged(float overdrive);
    void outputDeviceChanged(QAudioDevice audioDevice);

private:
    QMap<int, SoundEffect*> m_soundEffectMap{};
    QAudioDevice m_outputDevice{};

    float m_volume{ 1.f };
    float m_overdrive{ 0.f };
};

#endif // AUDIOSYSTEM_H
