#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include <QObject>
#include <QUrl>
#include <QMap>
#include <QAudioDevice>

class SoundEffect;
class AudioDevice;

class AudioSystem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AudioDevice* outputDevice READ outputDevice CONSTANT FINAL)

public:
    explicit AudioSystem(QObject* const parent = nullptr);
    ~AudioSystem();

    Q_INVOKABLE void play(const int id, const QUrl& path);
    Q_INVOKABLE void stop(const int id) const;
    Q_INVOKABLE void stopAll() const;

    AudioDevice* outputDevice() const;

signals:
    void soundStarted(int id);
    void soundStopped(int id);

private:
    QMap<int, SoundEffect*> m_soundEffectMap{};
    AudioDevice* m_outputDevice{};
};

#endif // AUDIOSYSTEM_H
