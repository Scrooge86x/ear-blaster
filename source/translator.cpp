#include "translator.h"

#include <QDir>
#include <QGuiApplication>
#include <QMap>

Translator::Translator(QObject* parent)
    : QObject{ parent }
{
    QDir resourceDir{ ":/i18n" };
    QStringList files{ resourceDir.entryList({ "*.qm" }) };

    using namespace Qt::Literals::StringLiterals;
    const auto prefixSize{ u"ear-blaster_"_s.length() };
    const auto suffixSize{ u".qm"_s.length() };

    for (const auto& file : files) {
        const auto languageSize{ file.length() - prefixSize - suffixSize };
        m_availableLanguages.push_back(file.mid(prefixSize, languageSize));
    }

    QGuiApplication::installTranslator(&m_translator);
}

QStringList Translator::availableLanguages() const
{
    return m_availableLanguages;
}


QString Translator::currentLanguage() const
{
    return m_currentLanguage;
}

void Translator::setCurrentLanguage(const QString& newLanguage)
{
    if (newLanguage == m_currentLanguage) {
        return;
    }

    if (!m_availableLanguages.contains(newLanguage)) {
        qWarning() << "Language was not found: " << newLanguage;
        return;
    }

    using namespace Qt::Literals::StringLiterals;
    if (!m_translator.load(u"ear-blaster_"_s + newLanguage, u":/i18n/"_s)) {
        qWarning() << "Failed to load translation:" << newLanguage;
        return;
    }

    m_currentLanguage = newLanguage;
    emit currentLanguageChanged();
}

QString Translator::getLanguageFullName(const QString& shortName) const
{
    using namespace Qt::Literals::StringLiterals;
    const QMap<QString, QString> languageMap{
        { u"en"_s, u"English"_s },
        { u"pl"_s, u"Polish"_s },
    };
    return languageMap.value(shortName, shortName);
}
