#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "soundplayer.h"

int main(int argc, char *argv[])
{
    // Sounds way better than the default
    qputenv("QT_MEDIA_BACKEND", "windows");

    // Fix for resize causing flickering
    qputenv("QT_D3D_NO_FLIP", "1" );

    QGuiApplication app{ argc, argv };

    QQmlApplicationEngine engine{};

    SoundPlayer soundPlayer{};
    engine.rootContext()->setContextProperty("soundPlayer", &soundPlayer);

    using namespace Qt::Literals::StringLiterals;
    engine.load(QUrl(u"qrc:/qt/qml/ear-blaster/qml/Main.qml"_s));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
