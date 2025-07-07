#ifndef SOUNDEFFECT_H
#define SOUNDEFFECT_H

#include <QObject>
#include <QUrl>
#include <QAudioDevice>
#include <QAudioBuffer>
#include <QtTypes>
#include <QThread>

class QAudioDecoder;
class QAudioSink;
class QIODevice;

class SoundEffect : public QObject
{
    Q_OBJECT

public:
    explicit SoundEffect();
    ~SoundEffect();

    void play(const QUrl& filePath);
    void stop();

    float volume() const;
    void setVolume(const float volume);

    float overdrive() const;
    void setOverdrive(const float overdrive);

    QAudioDevice outputDevice() const;
    void setOutputDevice(const QAudioDevice& outputDevice);

signals:
    void startedPlaying();
    void stoppedPlaying();

private:
    void processBuffer();

    QAudioDecoder* m_decoder{};
    QAudioSink* m_audioSink{};
    QIODevice* m_ioDevice{};

    QAudioBuffer m_currentBuffer{};
    qint64 m_bytesWritten{};

    QAudioDevice m_outputDevice{};
    float m_volume{ 1.f };
    float m_overdrive{};

    QThread m_thread{};
};

#endif // SOUNDEFFECT_H
