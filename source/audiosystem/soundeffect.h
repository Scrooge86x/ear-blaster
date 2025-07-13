#ifndef SOUNDEFFECT_H
#define SOUNDEFFECT_H

#include <QObject>
#include <QUrl>
#include <QAudioBuffer>
#include <QtTypes>
#include <QThread>

class QAudioDecoder;
class QAudioSink;
class QIODevice;
class AudioDevice;

class SoundEffect : public QObject
{
    Q_OBJECT

public:
    explicit SoundEffect(const AudioDevice& outputAudioDevice);
    ~SoundEffect();

    void play(const QUrl& filePath);
    void stop();

    const AudioDevice& outputDevice() const { return m_outputAudioDevice; }

    void invalidateAudioSink();

signals:
    void startedPlaying();
    void stoppedPlaying();

private:
    void processBuffer();
    void initAudioSink();

    QAudioDecoder* m_decoder{};
    QAudioSink* m_audioSink{};
    QIODevice* m_outputIODevice{};

    QAudioBuffer m_currentBuffer{};
    qint64 m_bytesWritten{};

    const AudioDevice& m_outputAudioDevice;

    QThread m_thread{};
};

#endif // SOUNDEFFECT_H
