#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include "microphonepassthrough.h"

#include <QObject>
#include <QUrl>
#include <QMap>

class SoundEffect;
class AudioDevice;

class AudioSystem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AudioDevice* outputDevice READ outputDevice CONSTANT FINAL)
    Q_PROPERTY(AudioDevice* inputDevice READ inputDevice CONSTANT FINAL)
    Q_PROPERTY(MicrophonePassthrough* micPassthrough READ micPassthrough CONSTANT FINAL)

public:
    explicit AudioSystem(QObject* const parent = nullptr);
    ~AudioSystem();

    Q_INVOKABLE void play(const int id, const QUrl& path);
    Q_INVOKABLE void stop(const int id) const;
    Q_INVOKABLE void stopAll() const;

    MicrophonePassthrough* micPassthrough() const { return m_micPassthrough; }
    AudioDevice* outputDevice() const { return m_outputDevice; }
    AudioDevice* inputDevice() const { return m_inputDevice; }

signals:
    void soundStarted(int id);
    void soundStopped(int id);

private:
    QMap<int, SoundEffect*> m_soundEffectMap{};
    MicrophonePassthrough* m_micPassthrough{};
    AudioDevice* m_outputDevice{};
    AudioDevice* m_inputDevice{};
};

#endif // AUDIOSYSTEM_H
