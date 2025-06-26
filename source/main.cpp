#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app{ argc, argv };

    QQmlApplicationEngine engine{};

    using namespace Qt::Literals::StringLiterals;
    engine.load(QUrl(u"qrc:/qt/qml/ear-blaster/qml/Main.qml"_s));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
