#include "texttospeech.h"

TextToSpeech::TextToSpeech(
    const AudioDevice& outputAudioDevice,
    const AudioDevice& monitorAudioDevice
)
    : m_outputAudioDevice{ outputAudioDevice }
    , m_monitorAudioDevice{ monitorAudioDevice }
    , m_audioOutput{ outputAudioDevice, {}, this }
    , m_monitorOutput{ monitorAudioDevice, {}, this }
{
    connect(&m_thread, &QThread::started, this, [this] {
        m_audioOutput.initialize();
        m_monitorOutput.initialize();
    });
    m_thread.start();
    this->moveToThread(&m_thread);
}

TextToSpeech::~TextToSpeech()
{
    if (m_thread.isRunning()) {
        m_thread.quit();
        m_thread.wait();
    }
}

void TextToSpeech::say(const QString& text)
{
    m_tts.say(text);
}

void TextToSpeech::stop()
{
    // .pause() instead of .stop() because it crashes the program as of Qt 6.9.1
    m_tts.pause(QTextToSpeech::BoundaryHint::Immediate);
}
