#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include "audiodevice.h"
#include "audioshared.h"

#include <QObject>
#include <QPointer>

class QIODevice;
class QAudioSink;

struct AudioOutputConfig {
    float bufferedMs{ 150.f };
    bool initializeVolume{ true };
    QAudioFormat format{ AudioShared::getAudioFormat() };
};

class AudioOutput : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(AudioOutput)

public:
    explicit AudioOutput(
        const AudioDevice& audioDevice,
        const AudioOutputConfig& config,
        QObject* const parent = nullptr
    );

    bool start();
    void stop();

    bool initialize();
    void invalidate();

    QAudioFormat format() const;
    void setFormat(const QAudioFormat& format);

    QAudioSink* audioSink() const;
    QIODevice* ioDevice() const;

signals:
    void initialized();

private slots:
    void onDeviceChanged();

private:
    AudioOutputConfig m_config{};
    QPointer<QAudioSink> m_audioSink{};
    QIODevice* m_ioDevice{};
    const AudioDevice& m_audioDevice;
};

#endif // AUDIOOUTPUT_H
