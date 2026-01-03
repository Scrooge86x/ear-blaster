#include "texttospeech.h"
#include "audioshared.h"

#include <QAudioFormat>
#include <QIODevice>
#include <QLoggingCategory>
#include <QAudioSink>
#include <QTimer>

Q_STATIC_LOGGING_CATEGORY(ttsLog, "ear-blaster-texttospeech")

TextToSpeech::TextToSpeech(
    const AudioDevice& outputAudioDevice,
    const AudioDevice& monitorAudioDevice
)
    : m_outputAudioDevice{ outputAudioDevice }
    , m_monitorAudioDevice{ monitorAudioDevice }
    , m_audioOutput{ outputAudioDevice, {}, this }
    , m_monitorOutput{ monitorAudioDevice, {}, this }
{
    connect(this, &TextToSpeech::say,
            this, &TextToSpeech::onSay);
    connect(this, &TextToSpeech::stop,
            this, &TextToSpeech::onStop);

    connect(&m_tts, &QTextToSpeech::localeChanged,
            this, &TextToSpeech::stop);
    connect(&m_tts, &QTextToSpeech::voiceChanged,
            this, &TextToSpeech::stop);

    connect(&m_tts, &QTextToSpeech::stateChanged, this, [this](const QTextToSpeech::State state) {
        if (state == QTextToSpeech::Ready && m_audioQueue.size()) {
            m_audioOutput.start();
            processQueue();
        }
    });

    // TODO: Actually preload the right format, this is a placeholder
    QAudioFormat format{};
    format.setChannelCount(2);
    format.setSampleFormat(QAudioFormat::Int16);
    format.setSampleRate(16'000);
    m_audioOutput.setFormat(format);

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

void TextToSpeech::onSay(const QString& text)
{
    if (text.isEmpty() || m_tts.state() != QTextToSpeech::Ready) {
        emit stop();
        return;
    }
    onStop();
    m_isPlaying = true;

    // TODO: Consider using some lockfree spsc queue instead
    // current solution has no performance problems so it's low priority
    m_tts.synthesize(text, this, [this](const QAudioBuffer& audioBuffer) {
        const QMutexLocker lock{ &m_mutex };
        // NOLINTNEXTLINE(clang-analyzer-core.CallAndMessage)
        m_audioQueue.enqueue(audioBuffer);
    });
}

void TextToSpeech::onStop()
{
    m_audioOutput.stop();
    m_audioQueue.clear();
    m_currentBuffer = {};
    m_bytesWritten = 0;
    m_isPlaying = false;
}

void TextToSpeech::processQueue()
{
    if (m_bytesWritten == -1) {
        if (m_audioQueue.empty()) {
            emit stop();
            return;
        }

        m_currentBuffer = m_audioQueue.dequeue();
        m_bytesWritten = 0;

        auto format{ m_currentBuffer.format() };
        if (format.channelCount() == 1) {
            m_currentBuffer = AudioShared::monoToStereo(m_currentBuffer);
            format.setChannelCount(2);
        }

        if (m_audioOutput.format() != format) {
            m_audioOutput.setFormat(format);
            if (!m_audioOutput.start()) {
                return;
            }
        }
    }

    QIODevice* const outputIODevice{ m_audioOutput.ioDevice() };
    if (!outputIODevice) {
        return;
    }

    const QAudioSink* const outputAudioSink{ m_audioOutput.audioSink() };
    const qint64 bytesToWrite{ std::min(
        outputAudioSink->bytesFree(), m_currentBuffer.byteCount() - m_bytesWritten
    ) };

    if (!outputAudioSink->bytesFree()) {
        return QTimer::singleShot(50, this, &TextToSpeech::processQueue);
    }

    m_bytesWritten += outputIODevice->write(m_currentBuffer.data<char>() + m_bytesWritten, bytesToWrite);

    if (m_bytesWritten == m_currentBuffer.byteCount()) {
        m_bytesWritten = -1;
    }
    return QTimer::singleShot(1, this, &TextToSpeech::processQueue);
}
