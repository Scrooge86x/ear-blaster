#ifndef GLOBALKEYLISTENER_H
#define GLOBALKEYLISTENER_H

#include <QObject>
#include <QString>

// Don't worry about the name, this class will be extended in the future
class GlobalKeyListener : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentSequence READ getCurrentSequence NOTIFY currentSequenceChanged)

public:
    explicit GlobalKeyListener(QObject* parent = nullptr);
    virtual ~GlobalKeyListener();

    static GlobalKeyListener& instance();
    QString getCurrentSequence() const;

signals:
    void currentSequenceChanged(const QString& hotkey);

private:
    friend void setCurrentSequence(const QString&);
    QString m_currentSequence{};
};

#endif // GLOBALKEYLISTENER_H
