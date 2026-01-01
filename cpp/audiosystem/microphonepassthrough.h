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
    Q_DISABLE_COPY(MicrophonePassthrough)

public:
    // No parent object because it's moving itself to a different thread
    explicit MicrophonePassthrough(const AudioDevice& outputAudioDevice);
    ~MicrophonePassthrough();

    AudioDevice* inputDevice() const { return m_inputAudioDevice; }

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
    const AudioDevice& m_outputAudioDevice;
};

#endif // MICROPHONEPASSTHROUGH_H
