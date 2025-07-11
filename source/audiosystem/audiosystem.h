#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include <QObject>
#include <QUrl>
#include <QMap>

Q_MOC_INCLUDE("source/audiosystem/microphonepassthrough.h")

class SoundEffect;
class AudioDevice;
class MicrophonePassthrough;

class AudioSystem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AudioDevice* outputDevice READ outputDevice CONSTANT FINAL)
    Q_PROPERTY(MicrophonePassthrough* micPassthrough READ micPassthrough CONSTANT FINAL)

public:
    explicit AudioSystem(QObject* const parent = nullptr);
    ~AudioSystem();

    Q_INVOKABLE void play(const int id, const QUrl& path);
    Q_INVOKABLE void stop(const int id) const;
    Q_INVOKABLE void stopAll() const;

    MicrophonePassthrough* micPassthrough() const { return m_micPassthrough; }
    AudioDevice* outputDevice() const { return m_outputDevice; }

signals:
    void soundStarted(int id);
    void soundStopped(int id);

private:
    QMap<int, SoundEffect*> m_soundEffectMap{};
    MicrophonePassthrough* m_micPassthrough{};
    AudioDevice* m_outputDevice{};
};

#endif // AUDIOSYSTEM_H
