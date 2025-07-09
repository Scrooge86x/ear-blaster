#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H

#include <QObject>
#include <QAudioDevice>

class AudioDevice : public QObject {
    Q_OBJECT
    Q_PROPERTY(QAudioDevice device READ device WRITE setDevice NOTIFY deviceChanged FINAL)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged FINAL)
    Q_PROPERTY(float overdrive READ overdrive WRITE setOverdrive NOTIFY overdriveChanged FINAL)

public:
    explicit AudioDevice(QObject* const parent = nullptr)
        : QObject{ parent }
    {}

    QAudioDevice device() const { return m_device; }
    void setDevice(const QAudioDevice& device);

    float volume() const { return m_volume; }
    void setVolume(const float volume);

    float overdrive() const { return m_overdrive; }
    void setOverdrive(const float overdrive);

signals:
    void deviceChanged();
    void volumeChanged(float volume);
    void overdriveChanged(float overdrive);

private:
    QAudioDevice m_device{};
    float m_volume{ 1.f };
    float m_overdrive{};
};

#endif // AUDIODEVICE_H
