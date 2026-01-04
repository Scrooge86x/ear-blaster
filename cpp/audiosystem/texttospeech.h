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
    Q_DISABLE_COPY_MOVE(TextToSpeech)

public:
    explicit TextToSpeech(
        const AudioDevice& outputAudioDevice,
        const AudioDevice& monitorAudioDevice
    );
    ~TextToSpeech();

    Q_INVOKABLE bool isPlaying() const { return m_isPlaying; }

    // Signals can't be used for these properties because TextToSpeech is on its own thread
    // The limitation comes from qml code not being able to connect to
    // signals that arent on the qml engine thread
    Q_INVOKABLE QLocale locale() const { return m_tts.locale(); }
    Q_INVOKABLE void setLocale(const QLocale& locale) { m_tts.setLocale(locale); }
    Q_INVOKABLE QVoice voice() const { return m_tts.voice(); }
    Q_INVOKABLE void setVoice(const QVoice& voice) { m_tts.setVoice(voice); }
    Q_INVOKABLE void setPitch(const double pitch);
    Q_INVOKABLE void setRate(const double rate);

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
