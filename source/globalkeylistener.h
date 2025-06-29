#ifndef GLOBALKEYLISTENER_H
#define GLOBALKEYLISTENER_H

#include <QObject>
#include <QString>

// Don't worry about the name, this class will be extended in the future
class GlobalKeyListener : public QObject
{
    Q_OBJECT
public:
    explicit GlobalKeyListener(QObject* parent = nullptr);

    Q_INVOKABLE QString sequenceToString(const Qt::Key key, const Qt::KeyboardModifiers modifiers) const;
};

#endif // GLOBALKEYLISTENER_H
