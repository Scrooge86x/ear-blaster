#ifndef GLOBALKEYLISTENER_H
#define GLOBALKEYLISTENER_H

#include <QObject>
#include <QString>

class GlobalKeyListener : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentSequence READ getCurrentSequence NOTIFY currentSequenceChanged)

public:
    explicit GlobalKeyListener(QObject* parent = nullptr);
    ~GlobalKeyListener();

    static GlobalKeyListener& instance();
    QString getCurrentSequence() const;

signals:
    void currentSequenceChanged(const QString& hotkey);

private:
    friend void setCurrentSequence(const QString&);
    QString m_currentSequence{};
};

#endif // GLOBALKEYLISTENER_H
