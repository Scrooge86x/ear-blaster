#include "texttospeech.h"

TextToSpeech::TextToSpeech(
    const AudioDevice& outputAudioDevice,
    const AudioDevice& monitorAudioDevice,
    QObject* const parent
)
    : QObject{ parent }
    , m_outputAudioDevice{ outputAudioDevice }
    , m_monitorAudioDevice{ monitorAudioDevice }
    , m_audioOutput{ outputAudioDevice, {}, this }
    , m_monitorOutput{ monitorAudioDevice, {}, this }
{}

void TextToSpeech::say(const QString& text)
{
    m_tts.say(text);
}

void TextToSpeech::stop()
{
    // .pause() instead of .stop() because it crashes the program as of Qt 6.9.1
    m_tts.pause(QTextToSpeech::BoundaryHint::Immediate);
}
