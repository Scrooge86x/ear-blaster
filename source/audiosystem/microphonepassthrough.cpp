#include "microphonepassthrough.h"

#include <QAudioDevice>
#include <QAudioSource>
#include <QAudioSink>

#include <QDebug>
#include <QCoreApplication>

static consteval QAudioFormat getAudioFormat() {
    QAudioFormat format{};
    format.setChannelCount(2);
    format.setSampleFormat(QAudioFormat::Int16);
    format.setSampleRate(48'000);
    return format;
}

template <QAudioFormat::SampleFormat> struct SampleFormatType { using type = void; };
template <> struct SampleFormatType<QAudioFormat::UInt8> { using type = quint8; };
template <> struct SampleFormatType<QAudioFormat::Int16> { using type = qint16; };
template <> struct SampleFormatType<QAudioFormat::Int32> { using type = qint32; };
template <> struct SampleFormatType<QAudioFormat::Float> { using type = float; };

using SampleType = SampleFormatType<getAudioFormat().sampleFormat()>::type;

MicrophonePassthrough::MicrophonePassthrough()
    : QObject{ nullptr }
{
    m_thread.start();
    this->moveToThread(&m_thread);
}

MicrophonePassthrough::~MicrophonePassthrough()
{
    if (m_audioSource) {
        QMetaObject::invokeMethod(m_audioSource, &QObject::deleteLater, Qt::BlockingQueuedConnection);
    }
    m_thread.quit();
    m_thread.wait();
}

void MicrophonePassthrough::start()
{
    QMetaObject::invokeMethod(this, [this]() {
        if (!m_audioSource || !m_audioSink) {
            return;
        }

        m_inputDevice = m_audioSource->start();
        m_outputDevice = m_audioSink->start();
        connect(m_inputDevice, &QIODevice::readyRead, this, &MicrophonePassthrough::processBuffer);
    }, Qt::QueuedConnection);
}

void MicrophonePassthrough::stop()
{
    QMetaObject::invokeMethod(this, [this]() {
        if (!m_audioSource || !m_audioSink) {
            return;
        }

        m_audioSource->reset();
        m_audioSource->stop();
        m_inputDevice = nullptr;

        m_audioSink->reset();
        m_audioSink->stop();
        m_outputDevice = nullptr;
    }, Qt::QueuedConnection);
}

float MicrophonePassthrough::volume() const
{
    return m_volume;
}

void MicrophonePassthrough::setVolume(const float volume)
{
    m_volume = volume;
}

float MicrophonePassthrough::overdrive() const
{
    return m_overdrive;
}

void MicrophonePassthrough::setOverdrive(const float overdrive)
{
    m_overdrive = overdrive;
}

void MicrophonePassthrough::setOutputDevice(const QAudioDevice& outputDevice)
{
    QMetaObject::invokeMethod(this, [this, outputDevice]() {
        if (m_audioSink) {
            m_audioSink->reset();
            m_audioSink->stop();
            m_outputDevice = nullptr;
            delete m_audioSink;
        }

        m_audioSink = new QAudioSink{ outputDevice, getAudioFormat(), this };
        m_audioSink->setVolume(m_volume);
    }, Qt::QueuedConnection);
}

void MicrophonePassthrough::setInputDevice(const QAudioDevice& inputDevice)
{
    QMetaObject::invokeMethod(this, [this, inputDevice]() {
        if (m_audioSource) {
            m_audioSource->reset();
            m_audioSource->stop();
            m_inputDevice = nullptr;
            delete m_audioSource;
        }

        m_audioSource = new QAudioSource{ inputDevice, getAudioFormat(), this };
    }, Qt::QueuedConnection);
}

void MicrophonePassthrough::processBuffer()
{
    m_outputDevice->write(m_inputDevice->readAll());
}
