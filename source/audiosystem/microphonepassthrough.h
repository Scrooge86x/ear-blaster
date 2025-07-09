#ifndef MICROPHONEPASSTHROUGH_H
#define MICROPHONEPASSTHROUGH_H

#include <QObject>
#include <QtTypes>
#include <QThread>

class QAudioDevice;
class QAudioSource;
class QAudioSink;

class MicrophonePassthrough : public QObject
{
    Q_OBJECT

public:
    explicit MicrophonePassthrough();
    ~MicrophonePassthrough();

    void start();
    void stop();

    float volume() const;
    void setVolume(const float volume);

    float overdrive() const;
    void setOverdrive(const float overdrive);

    void setInputDevice(const QAudioDevice& inputAudioDevice);
    void setOutputDevice(const QAudioDevice& outputAudioDevice);
signals:

private:
    void processBuffer();

    qint64 m_bytesWritten{};

    QIODevice* m_inputDevice{};
    QIODevice* m_outputDevice{};

    QAudioSource* m_audioSource{};
    QAudioSink* m_audioSink{};

    float m_volume{ 1.f };
    float m_overdrive{};

    QThread m_thread{};
};

#endif // MICROPHONEPASSTHROUGH_H
