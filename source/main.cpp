#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>

#include "soundplayer.h"
#include "globalkeylistener.h"

int main(int argc, char *argv[])
{
    // Sounds way better than the default
    qputenv("QT_MEDIA_BACKEND", "windows");

    // Fix for resize causing flickering
    qputenv("QT_D3D_NO_FLIP", "1" );

    // Qt documentation says that QApplication should be used instead
    // because of Qt Labs Platform, but this would disable the dark titlebar
    // on windows 10 so for now it will stay as QGuiApplication
    QGuiApplication app{ argc, argv };

    // The icon already set from the windows rc file, but in order for
    // ColorDialog to pick it up, it needs to also be set here
    app.setWindowIcon(QIcon{ ":/qt/qml/ear-blaster/resources/ear-blaster.ico" });

    QQmlApplicationEngine engine{};

    SoundPlayer soundPlayer{};
    engine.rootContext()->setContextProperty("soundPlayer", &soundPlayer);
    engine.rootContext()->setContextProperty("globalKeyListener", &GlobalKeyListener::instance());

    using namespace Qt::Literals::StringLiterals;
    engine.load(QUrl(u"qrc:/qt/qml/ear-blaster/qml/Main.qml"_s));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
