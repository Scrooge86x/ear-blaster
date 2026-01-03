#ifndef AUDIOSHARED_H
#define AUDIOSHARED_H

#include <QAudioFormat>
#include <QtAudio>
#include <QAudioBuffer>

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

inline QAudioBuffer monoToStereo(const QAudioBuffer& monoBuffer)
{
    const QAudioFormat monoBufferFormat{ monoBuffer.format() };
    const int frameCount{ static_cast<int>(monoBuffer.frameCount()) };

    QAudioFormat stereoBufferFormat{ monoBufferFormat };
    stereoBufferFormat.setChannelCount(2);

    // Hope for NRVO...
    QAudioBuffer stereoBuffer{ frameCount, stereoBufferFormat };

    if (!monoBuffer.isValid()
            || !stereoBuffer.isValid()
            || monoBufferFormat.channelCount() != 1) {
        return stereoBuffer;
    }

    const auto monoBufferToStereoBuffer = [&monoBuffer, &stereoBuffer, &frameCount]<typename T>() {
        const T* const monoData{ monoBuffer.constData<T>() };
        T* const stereoData{ stereoBuffer.data<T>() };
        for (int i{}; i < frameCount; ++i) {
            stereoData[i * 2 + 0] = monoData[i];
            stereoData[i * 2 + 1] = monoData[i];
        }
    };

    switch (monoBufferFormat.sampleFormat()) {
        case QAudioFormat::UInt8:
            monoBufferToStereoBuffer.template operator()<quint8>();
            break;
        case QAudioFormat::Int16:
            monoBufferToStereoBuffer.template operator()<qint16>();
            break;
        case QAudioFormat::Int32:
            monoBufferToStereoBuffer.template operator()<qint32>();
            break;
        case QAudioFormat::Float:
            monoBufferToStereoBuffer.template operator()<float>();
            break;
        default:
            break;
    }

    return stereoBuffer;
}

}

#endif // AUDIOSHARED_H
