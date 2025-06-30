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
    virtual ~GlobalKeyListener();

    static GlobalKeyListener& instance();
    Q_INVOKABLE QString sequenceToString(const Qt::Key key, const Qt::KeyboardModifiers modifiers) const;

signals:
    void globalHotkeyPressed(const QString hotkey);
};

#endif // GLOBALKEYLISTENER_H
