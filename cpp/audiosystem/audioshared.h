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

inline void addOverdrive(
    char* samples,
    const QAudioFormat::SampleFormat sampleFormat,
    const qint64 numSamples,
    const float boostLevel
) {
    const auto applyOverdrive = [&samples, &numSamples, &boostLevel]<typename T>() {
        constexpr auto minVal{ std::numeric_limits<T>::min() };
        constexpr auto maxVal{ std::numeric_limits<T>::max() };
        const double gain{ 1.0 + QtAudio::convertVolume(
            boostLevel, QtAudio::LogarithmicVolumeScale, QtAudio::LinearVolumeScale
        ) * 150.0 };

        T* tSamples{ reinterpret_cast<T*>(samples) };
        for (qint64 i{}; i < numSamples; ++i) {
            double boosted{ static_cast<double>(*tSamples) * gain };
            *tSamples++ = std::clamp<double>(boosted, minVal, maxVal);
        }
    };

    switch (sampleFormat) {
    case QAudioFormat::UInt8:
        applyOverdrive.template operator()<quint8>();
        break;
    case QAudioFormat::Int16:
        applyOverdrive.template operator()<qint16>();
        break;
    case QAudioFormat::Int32:
        applyOverdrive.template operator()<qint32>();
        break;
    case QAudioFormat::Float:
        applyOverdrive.template operator()<float>();
        break;
    default:
        break;
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
