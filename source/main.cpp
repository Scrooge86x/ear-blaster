#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QLoggingCategory>

#include "globalkeylistener.h"
#include "translator.h"
#include "audiosystem/audiosystem.h"

#include <QMediaDevices>
#include "audiosystem/microphonepassthrough.h"
#include "audiosystem/audiodevice.h"

int main(int argc, char *argv[])
{
    // Fix for resize causing flickering
    qputenv("QT_D3D_NO_FLIP", "1" );

    using namespace Qt::Literals::StringLiterals;
    QLoggingCategory::setFilterRules(u"qt.multimedia.ffmpeg.*=false"_s);

    // Qt documentation says that QApplication should be used instead
    // because of Qt Labs Platform, but this would disable the dark titlebar
    // on windows 10 so for now it will stay as QGuiApplication
    QGuiApplication app{ argc, argv };

    // The icon already set from the windows rc file, but in order for
    // ColorDialog to pick it up, it needs to also be set here
    app.setWindowIcon(QIcon{ u":/qt/qml/ear-blaster/resources/ear-blaster.ico"_s });

    AudioDevice inputAudioDevice{};
    inputAudioDevice.setDevice(QMediaDevices::defaultAudioInput());
    AudioDevice outputAudioDevice{};
    outputAudioDevice.setDevice(QMediaDevices::defaultAudioOutput());

    MicrophonePassthrough micPassthrough{};
    micPassthrough.setInputDevice(&inputAudioDevice);
    micPassthrough.setOutputDevice(&outputAudioDevice);
    micPassthrough.start();

    AudioSystem audioSystem{ &app };
    Translator translator{};
    QQmlApplicationEngine engine{};
    QObject::connect(
        &translator, &Translator::currentLanguageChanged,
        &engine, &QQmlApplicationEngine::retranslate
    );

    engine.rootContext()->setContextProperty(u"audioSystem"_s, &audioSystem);
    engine.rootContext()->setContextProperty(u"translator"_s, &translator);
    engine.rootContext()->setContextProperty(u"globalKeyListener"_s, &GlobalKeyListener::instance());

    engine.load(u"qrc:/qt/qml/ear-blaster/qml/Main.qml"_s);
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
