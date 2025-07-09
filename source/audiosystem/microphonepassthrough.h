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

public:
    explicit MicrophonePassthrough();
    ~MicrophonePassthrough();

    void start();
    void stop();

    const AudioDevice* inputDevice() const { return m_inputAudioDevice; }
    void setInputDevice(const AudioDevice* const inputAudioDevice);

    const AudioDevice* outputDevice() const { return m_outputAudioDevice; }
    void setOutputDevice(const AudioDevice* const outputAudioDevice);
signals:

private:
    void processBuffer();

    QIODevice* m_inputDevice{};
    QIODevice* m_outputDevice{};

    QAudioSource* m_audioSource{};
    QAudioSink* m_audioSink{};

    const AudioDevice* m_inputAudioDevice{};
    const AudioDevice* m_outputAudioDevice{};

    QThread m_thread{};
};

#endif // MICROPHONEPASSTHROUGH_H
