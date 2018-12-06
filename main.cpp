#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "game.h"
#include "timer.h"
//#include "tile.h"
#include <qqml.h>
#include <QQuickView>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    //Expor as classes C++
    qmlRegisterType<Game>("CustomComponents", 1, 0, "Game");

    QQmlApplicationEngine engine; //definir a aplicação em main.qml
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
