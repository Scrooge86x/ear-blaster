#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QVariantList>
#include <QKeySequence>

class ConfigManager : public QObject
{
    Q_OBJECT
public:
    explicit ConfigManager(QObject *parent = nullptr);

    Q_INVOKABLE void loadConfig(const QString& filePath);
    Q_INVOKABLE void saveConfig(const QString& filePath);
    Q_INVOKABLE void addSound(const QString& name, const QString& filePath, const QKeySequence& sequence);

signals:
    void configLoaded(const QVariantList& config);
};

#endif // CONFIGMANAGER_H
