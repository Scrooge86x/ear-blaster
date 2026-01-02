#ifndef TEXTTOSPEECH_H
#define TEXTTOSPEECH_H

#include "audiooutput.h"

#include <QObject>
#include <QTextToSpeech>
#include <QLocale>
#include <QVoice>

class AudioDevice;

class TextToSpeech : public QObject {
    Q_OBJECT
    Q_PROPERTY(QTextToSpeech::State state READ state CONSTANT)
    Q_PROPERTY(QLocale locale READ locale WRITE setLocale CONSTANT)
    Q_PROPERTY(QVoice voice READ voice WRITE setVoice CONSTANT)

public:
    explicit TextToSpeech(
        const AudioDevice& outputAudioDevice,
        const AudioDevice& monitorAudioDevice,
        QObject* const parent = nullptr
    );

    Q_INVOKABLE void say(const QString& text);
    Q_INVOKABLE void stop();

    QTextToSpeech::State state() const { return m_tts.state(); }

    QLocale locale() const { return m_tts.locale(); }
    void setLocale(const QLocale& locale) { m_tts.setLocale(locale); }

    QVoice voice() const { return m_tts.voice(); }
    void setVoice(const QVoice& voice) { m_tts.setVoice(voice); }

    Q_INVOKABLE QList<QLocale> availableLocales() const { return m_tts.availableLocales(); }
    Q_INVOKABLE QList<QVoice> availableVoices() const { return m_tts.availableVoices(); }

private:
    AudioOutput m_audioOutput;
    AudioOutput m_monitorOutput;

    const AudioDevice& m_outputAudioDevice;
    const AudioDevice& m_monitorAudioDevice;

    QTextToSpeech m_tts{};
};

#endif // TEXTTOSPEECH_H
