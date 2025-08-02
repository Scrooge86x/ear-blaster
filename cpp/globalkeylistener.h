#ifndef GLOBALKEYLISTENER_H
#define GLOBALKEYLISTENER_H

#include <QObject>
#include <QString>

class GlobalKeyListener : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentSequence READ getCurrentSequence NOTIFY currentSequenceChanged)
    Q_DISABLE_COPY(GlobalKeyListener)

public:
    static GlobalKeyListener& instance();
    QString getCurrentSequence() const;

private:
    explicit GlobalKeyListener(QObject* parent = nullptr);
    ~GlobalKeyListener();

signals:
    void currentSequenceChanged(const QString& hotkey);

private:
    friend void setCurrentSequence(const QString&);
    QString m_currentSequence{};
};

#endif // GLOBALKEYLISTENER_H
