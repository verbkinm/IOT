#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "client.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName("VMS");
    app.setApplicationName("IOTV_Client");
    app.setApplicationVersion("0.6.3");

    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:/");
    Client client;
    engine.rootContext()->setContextProperty("client", &client);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
