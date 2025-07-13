#ifndef AUDIOSHARED_H
#define AUDIOSHARED_H

#include <QAudioFormat>
#include <QtAudio>

namespace AudioShared {

consteval QAudioFormat getAudioFormat() {
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

template <typename T>
void addOverdrive(
    T* samples,
    const qint64 numSamples,
    const float boostLevel
) {
    constexpr auto minVal{ std::numeric_limits<SampleType>::min() };
    constexpr auto maxVal{ std::numeric_limits<SampleType>::max() };
    const double gain{ 1.0 + QtAudio::convertVolume(
        boostLevel, QtAudio::LogarithmicVolumeScale, QtAudio::LinearVolumeScale
    ) * 150.0 };

    for (qint64 i{}; i < numSamples; ++i) {
        double boosted{ static_cast<double>(*samples) * gain };
        *samples++ = std::clamp<double>(boosted, minVal, maxVal);
    }
}

}

#endif // AUDIOSHARED_H
