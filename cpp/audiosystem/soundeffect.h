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
    Q_DISABLE_COPY(SoundEffect)

public:
    explicit SoundEffect(
        const AudioDevice& outputAudioDevice,
        const AudioDevice& mmonitorAudioDevice
    );
    ~SoundEffect();

    void play(const QUrl& filePath);

    const AudioDevice& outputDevice() const { return m_outputAudioDevice; }

signals:
    void stopRequested();
    void startedPlaying();
    void stoppedPlaying();

private:
    void stop();
    void processBuffer();

    void initAudioOutputSink();
    void invalidateAudioOutputSink();
    void initAudioMonitorSink();
    void invalidateAudioMonitorSink();

    QAudioDecoder* m_decoder{};

    QAudioSink* m_outputAudioSink{};
    QAudioSink* m_monitorAudioSink{};

    QIODevice* m_outputIODevice{};
    QIODevice* m_monitorIODevice{};

    QAudioBuffer m_currentBuffer{};
    qint64 m_bytesWritten{};

    const AudioDevice& m_outputAudioDevice;
    const AudioDevice& m_monitorAudioDevice;

    QThread m_thread{};
};

#endif // SOUNDEFFECT_H
