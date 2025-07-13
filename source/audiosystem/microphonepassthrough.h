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
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged FINAL)
    Q_PROPERTY(AudioDevice* inputDevice READ inputDevice CONSTANT FINAL)
    Q_PROPERTY(AudioDevice* outputDevice READ outputDevice CONSTANT FINAL)

public:
    explicit MicrophonePassthrough();
    ~MicrophonePassthrough();

    bool enabled() const { return m_enabled; }
    void setEnabled(const bool enabled);

    AudioDevice* inputDevice() const { return m_inputAudioDevice; }
    AudioDevice* outputDevice() const { return m_outputAudioDevice; }

signals:
    void enabledChanged(bool enabled);

private:
    void start();
    void stop();

    void initAudioSink();
    void invalidateAudioSink();
    void initAudioSource();
    void invalidateAudioSource();

    void processBuffer();

    bool m_enabled{};
    QThread m_thread{};

    QIODevice* m_inputIODevice{};
    QIODevice* m_outputIODevice{};

    QAudioSource* m_audioSource{};
    QAudioSink* m_audioSink{};

    AudioDevice* m_inputAudioDevice{};
    AudioDevice* m_outputAudioDevice{};
};

#endif // MICROPHONEPASSTHROUGH_H
