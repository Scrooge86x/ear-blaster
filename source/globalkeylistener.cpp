#include "globalkeylistener.h"

#include <QKeySequence>

GlobalKeyListener::GlobalKeyListener(QObject* parent)
    : QObject{ parent }
{}

QString GlobalKeyListener::sequenceToString(const Qt::Key key, const Qt::KeyboardModifiers modifiers) const
{
    switch (key) {
    case Qt::Key_Control:
    case Qt::Key_Shift:
    case Qt::Key_Alt:
    case Qt::Key_Meta:
        return QKeySequence{ modifiers }.toString();
    default:
        if (modifiers.testFlag(Qt::KeypadModifier)) {
            return QKeySequence{ key | modifiers }.toString().replace("Num+", "Num");
        } else {
            return QKeySequence{ key | modifiers }.toString();
        }
    }
}
