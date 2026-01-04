#include "texttospeech.h"
#include "audioshared.h"

#include <QAudioFormat>
#include <QIODevice>
#include <QLoggingCategory>
#include <QAudioSink>
#include <QTimer>

#include <cmath>

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

    // While the sound will stop itself on deviceChanged the stop signal
    // must be emitted so the gui can synchronize
    connect(&m_outputAudioDevice, &AudioDevice::deviceChanged,
            this, &TextToSpeech::stop);

    connect(&m_monitorAudioDevice, &AudioDevice::enabledChanged, this, [this](const bool enabled) {
        if (m_audioOutput.ioDevice()) {
            m_monitorOutput.start();
        }
    });

    connect(&m_tts, &QTextToSpeech::stateChanged, this, [this](const QTextToSpeech::State state) {
        if (state == QTextToSpeech::Ready && m_audioQueue.size()) {
            m_audioOutput.start();
            m_monitorOutput.start();
            processQueue();
        }
    });

    connect(&m_thread, &QThread::started, this, [this] {
        // HACK
        // the call to QTextToSpeech::synthesize causes the playback to be
        // laggy for the whole runtime (somehow lags the engine used by the backend?)
        // the only solutions I've found after trying everything was to call QTextToSpeech::synthesize
        // after around 3 seconds, but I don't like magic numbers that could be different on another pc
        // and the other solution is what you see below...
        // undocumented backend magic like always with less popular features in Qt
        m_tts.setEngine("mock");
        m_tts.setEngine("sapi");
        emit engineInitialized();
        // HACK END

        // Preload the right format so there is no delay on the first playback
        using namespace Qt::Literals::StringLiterals;
        m_tts.synthesize(u"a"_s, this, [this, isInitialized = false](const QAudioBuffer& audioBuffer) mutable {
            if (isInitialized) {
                return;
            }
            isInitialized = true;

            auto format{ audioBuffer.format() };
            if (format.channelCount() == 1) {
                format.setChannelCount(2);
            }

            QMetaObject::invokeMethod(this, [this, format]() {
                m_audioOutput.setFormat(format);
                m_monitorOutput.setFormat(format);
            }, Qt::QueuedConnection);
        });
    });

    this->moveToThread(&m_thread);
    m_thread.start();

    // Wait for the engine to initialize to prevent a race condition with AppSettings
    QEventLoop waitingLoop{};
    connect(this, &TextToSpeech::engineInitialized,
            &waitingLoop, &QEventLoop::quit);
    waitingLoop.exec();
}

TextToSpeech::~TextToSpeech()
{
    if (m_thread.isRunning()) {
        m_thread.quit();
        m_thread.wait();
    }
}

void TextToSpeech::setPitch(const double pitch)
{
    emit stop();

    // For sapi at least any more precision than one decimal place
    // causes the engine to not synthesize at all (not documented btw)
    m_tts.setPitch(std::round(pitch * 10.0) / 10.0);
}

void TextToSpeech::setRate(const double rate)
{
    emit stop();
    m_tts.setRate(rate);
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
    m_monitorOutput.stop();
    m_audioQueue.clear();
    m_currentBuffer = {};
    m_bytesWritten = -1;
    m_isPlaying = false;
}

// TODO: Resolve code duplication with SoundEffect::processBuffer
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

        // According to https://doc.qt.io/qt-6/qtexttospeech.html#synthesize
        // the format can technically change so it must be handled unfortunately
        if (m_audioOutput.format() != format) {
            m_audioOutput.setFormat(format);
            if (!m_audioOutput.start()) {
                return;
            }
            m_monitorOutput.setFormat(format);
            m_monitorOutput.start();
        }
    }

    QIODevice* const outputIODevice{ m_audioOutput.ioDevice() };
    if (!outputIODevice) {
        return;
    }

    const QAudioSink* const outputAudioSink{ m_audioOutput.audioSink() };
    if (!outputAudioSink->bytesFree()) {
        return QTimer::singleShot(50, this, &TextToSpeech::processQueue);
    }

    const int bytesPerSample{ m_audioOutput.format().bytesPerSample() };
    if (!bytesPerSample) {
        return;
    }

    const qint64 bytesToWrite{ std::min(
        outputAudioSink->bytesFree(), m_currentBuffer.byteCount() - m_bytesWritten
    ) };
    const qint64 numSamples{ bytesToWrite / bytesPerSample };

    QIODevice* const monitorIODevice{ m_monitorOutput.ioDevice() };
    if (monitorIODevice) {
        const QAudioSink* const monitorAudioSink{ m_monitorOutput.audioSink() };

        // Synchronize devices if monitorAudioSink was just started
        if (monitorAudioSink->bytesFree() == monitorAudioSink->bufferSize()) {
            QByteArray padding{ outputAudioSink->bufferSize() - outputAudioSink->bytesFree(), '\0' };
            monitorIODevice->write(padding);
        }

        QByteArray samplesCopy{ m_currentBuffer.data<char>() + m_bytesWritten, bytesToWrite };
        if (m_monitorAudioDevice.overdrive()) {
            AudioShared::addOverdrive(
                samplesCopy.data(),
                m_audioOutput.format().sampleFormat(),
                numSamples,
                m_monitorAudioDevice.overdrive()
            );
        }
        monitorIODevice->write(samplesCopy);
    }

    if (m_outputAudioDevice.overdrive()) {
        AudioShared::addOverdrive(
            m_currentBuffer.data<char>() + m_bytesWritten,
            m_audioOutput.format().sampleFormat(),
            numSamples,
            m_outputAudioDevice.overdrive()
        );
    }
    m_bytesWritten += outputIODevice->write(m_currentBuffer.data<char>() + m_bytesWritten, bytesToWrite);

    if (m_bytesWritten == m_currentBuffer.byteCount()) {
        m_bytesWritten = -1;
    }
    return QTimer::singleShot(1, this, &TextToSpeech::processQueue);
}
