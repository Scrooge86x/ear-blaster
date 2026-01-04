#ifndef TEXTTOSPEECH_H
#define TEXTTOSPEECH_H

#include "audiooutput.h"

#include <QObject>
#include <QTextToSpeech>
#include <QLocale>
#include <QVoice>
#include <QThread>

#include <QQueue>
#include <QAudioBuffer>
#include <QMutex>

class AudioDevice;

class TextToSpeech : public QObject {
    Q_OBJECT
    // Signals are not connected to the properties because TextToSpeech is on its own thread
    // qml objects shouldn't be able to access them
    Q_PROPERTY(QLocale locale READ locale WRITE setLocale CONSTANT FINAL)
    Q_PROPERTY(QVoice voice READ voice WRITE setVoice CONSTANT FINAL)
    Q_DISABLE_COPY_MOVE(TextToSpeech)

public:
    explicit TextToSpeech(
        const AudioDevice& outputAudioDevice,
        const AudioDevice& monitorAudioDevice
    );
    ~TextToSpeech();

    Q_INVOKABLE bool isPlaying() const { return m_isPlaying; }

    QLocale locale() const { return m_tts.locale(); }
    void setLocale(const QLocale& locale) { m_tts.setLocale(locale); }

    QVoice voice() const { return m_tts.voice(); }
    void setVoice(const QVoice& voice) { m_tts.setVoice(voice); }

    Q_INVOKABLE QList<QLocale> availableLocales() const { return m_tts.availableLocales(); }
    Q_INVOKABLE QList<QVoice> availableVoices() const { return m_tts.availableVoices(); }

signals:
    void say(const QString& text);
    void stop();
    void engineInitialized();

private:
    void onSay(const QString& text);
    void onStop();

    void onSynthesisChunk(const QAudioBuffer& audioBuffer);
    void processQueue();

    AudioOutput m_audioOutput;
    AudioOutput m_monitorOutput;

    const AudioDevice& m_outputAudioDevice;
    const AudioDevice& m_monitorAudioDevice;

    QTextToSpeech m_tts{};
    QThread m_thread{};

    QAudioBuffer m_currentBuffer{};
    qint64 m_bytesWritten{ -1 };

    QQueue<QAudioBuffer> m_audioQueue{};
    QMutex m_mutex{};
    bool m_isPlaying{};
};

#endif // TEXTTOSPEECH_H
