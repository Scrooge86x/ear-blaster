#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include <QObject>
#include <QUrl>
#include <QMap>
#include <QAudioDevice>

#include <memory>

Q_MOC_INCLUDE("microphonepassthrough.h")
Q_MOC_INCLUDE("texttospeech.h")

class SoundEffect;
class AudioDevice;
class MicrophonePassthrough;
class TextToSpeech;

class AudioSystem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AudioDevice* outputDevice READ outputDevice CONSTANT FINAL)
    Q_PROPERTY(AudioDevice* monitorDevice READ monitorDevice CONSTANT FINAL)
    Q_PROPERTY(MicrophonePassthrough* micPassthrough READ micPassthrough CONSTANT FINAL)
    Q_PROPERTY(TextToSpeech* tts READ tts CONSTANT FINAL)
    Q_PROPERTY(QList<QAudioDevice> audioInputs READ audioInputs NOTIFY audioInputsChanged FINAL)
    Q_PROPERTY(QList<QAudioDevice> audioOutputs READ audioOutputs NOTIFY audioOutputsChanged FINAL)
    Q_DISABLE_COPY(AudioSystem)

public:
    explicit AudioSystem(QObject* const parent = nullptr);
    ~AudioSystem();

    Q_INVOKABLE void play(const int id, const QUrl& path);
    Q_INVOKABLE void stop(const int id) const;
    Q_INVOKABLE void stopAll() const;

    MicrophonePassthrough* micPassthrough() const { return m_micPassthrough.get(); }
    AudioDevice* outputDevice() const { return m_outputAudioDevice; }
    AudioDevice* monitorDevice() const { return m_monitorAudioDevice; }

    TextToSpeech* tts() { return m_tts.get(); }

    static QList<QAudioDevice> audioInputs();
    static QList<QAudioDevice> audioOutputs();

    Q_INVOKABLE static QAudioDevice getInputDeviceById(const QString& id);
    Q_INVOKABLE static QAudioDevice getOutputDeviceById(const QString& id);

    Q_INVOKABLE static qsizetype getInputDeviceIndexById(const QString& id);
    Q_INVOKABLE static qsizetype getOutputDeviceIndexById(const QString& id);

signals:
    void soundStarted(int id);
    void soundStopped(int id);
    void audioInputsChanged();
    void audioOutputsChanged();

private:
    QMap<int, SoundEffect*> m_soundEffectMap{};
    AudioDevice* m_outputAudioDevice{};
    AudioDevice* m_monitorAudioDevice{};
    std::unique_ptr<MicrophonePassthrough> m_micPassthrough{};
    std::unique_ptr<TextToSpeech> m_tts{};
};

#endif // AUDIOSYSTEM_H
