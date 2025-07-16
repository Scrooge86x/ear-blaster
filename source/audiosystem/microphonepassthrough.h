#ifndef MICROPHONEPASSTHROUGH_H
#define MICROPHONEPASSTHROUGH_H

#include <QObject>
#include <QThread>

class QAudioDevice;
class QAudioSource;
class QAudioSink;
class AudioDevice;

class MicrophonePassthrough : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AudioDevice* inputDevice READ inputDevice CONSTANT FINAL)
    Q_PROPERTY(AudioDevice* outputDevice READ outputDevice CONSTANT FINAL)

public:
    explicit MicrophonePassthrough();
    ~MicrophonePassthrough();

    AudioDevice* inputDevice() const { return m_inputAudioDevice; }
    AudioDevice* outputDevice() const { return m_outputAudioDevice; }

private:
    void initAudioSink();
    void invalidateAudioSink();
    void initAudioSource();
    void invalidateAudioSource();

    void processBuffer();

    QThread m_thread{};

    QIODevice* m_inputIODevice{};
    QIODevice* m_outputIODevice{};

    QAudioSource* m_audioSource{};
    QAudioSink* m_audioSink{};

    AudioDevice* m_inputAudioDevice{};
    AudioDevice* m_outputAudioDevice{};
};

#endif // MICROPHONEPASSTHROUGH_H
