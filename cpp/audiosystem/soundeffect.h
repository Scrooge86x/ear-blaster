#ifndef SOUNDEFFECT_H
#define SOUNDEFFECT_H

#include "audiooutput.h"

#include <QObject>
#include <QUrl>
#include <QAudioBuffer>
#include <QtTypes>
#include <QThread>

class QAudioDecoder;
class AudioDevice;

class SoundEffect : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(SoundEffect)

public:
    explicit SoundEffect(
        const AudioDevice& outputAudioDevice,
        const AudioDevice& mmonitorAudioDevice
    );
    ~SoundEffect();

    void play(const QUrl& filePath);

signals:
    void stopRequested();
    void startedPlaying();
    void stoppedPlaying();

private slots:
    void onAudioOutputInit();

private:
    void stop();
    void processBuffer();

    QAudioDecoder* m_decoder{};

    AudioOutput m_audioOutput;
    AudioOutput m_monitorOutput;

    QAudioBuffer m_currentBuffer{};
    qint64 m_bytesWritten{};

    const AudioDevice& m_outputAudioDevice;
    const AudioDevice& m_monitorAudioDevice;

    QThread m_thread{};
};

#endif // SOUNDEFFECT_H
