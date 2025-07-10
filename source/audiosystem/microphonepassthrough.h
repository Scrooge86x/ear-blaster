#ifndef MICROPHONEPASSTHROUGH_H
#define MICROPHONEPASSTHROUGH_H

#include <QObject>
#include <QtTypes>
#include <QThread>

class QAudioDevice;
class QAudioSource;
class QAudioSink;
class AudioDevice;

class MicrophonePassthrough : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged FINAL)

public:
    explicit MicrophonePassthrough();
    ~MicrophonePassthrough();

    bool enabled() const { return m_enabled; }
    void setEnabled(const bool enabled);

    const AudioDevice* inputDevice() const { return m_inputAudioDevice; }
    void setInputDevice(const AudioDevice* const inputAudioDevice);

    const AudioDevice* outputDevice() const { return m_outputAudioDevice; }
    void setOutputDevice(const AudioDevice* const outputAudioDevice);

signals:
    void enabledChanged(bool enabled);

private:
    void start();
    void stop();
    void processBuffer();

    bool m_enabled{};
    QThread m_thread{};

    QIODevice* m_inputDevice{};
    QIODevice* m_outputDevice{};

    QAudioSource* m_audioSource{};
    QAudioSink* m_audioSink{};

    const AudioDevice* m_inputAudioDevice{};
    const AudioDevice* m_outputAudioDevice{};
};

#endif // MICROPHONEPASSTHROUGH_H
