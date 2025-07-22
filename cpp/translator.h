#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTranslator>

class Translator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentLanguage READ currentLanguage WRITE setCurrentLanguage NOTIFY currentLanguageChanged FINAL)
    Q_PROPERTY(QStringList availableLanguages READ availableLanguages CONSTANT FINAL)

public:
    explicit Translator(QObject *parent = nullptr);

    QStringList availableLanguages() const;

    QString currentLanguage() const;
    void setCurrentLanguage(const QString& newLanguage);

    Q_INVOKABLE QString getLanguageFullName(const QString& shortName) const;

signals:
    void currentLanguageChanged();

private:
    QStringList m_availableLanguages{};
    QString m_currentLanguage{};
    QTranslator m_translator{};
};

#endif // TRANSLATOR_H
