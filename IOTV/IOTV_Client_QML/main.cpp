#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "client.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

//    qmlRegisterType<Client>("io.qt.Backend_Client", 1, 0, "Backend_Client");
    qmlRegisterType<Device>("io.qt.Backend_Device", 1, 0, "Backend_Device");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    Client client;
    engine.rootContext()->setContextProperty("client", &client);

    return app.exec();
}
