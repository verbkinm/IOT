#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "server.h"
#include "wrapper_device.h"
#include "wrapper_raw.h"


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    app.setOrganizationName("VMS");
//    app.setOrganizationDomain("somecompany.com");
    app.setApplicationName("IOTV_Client_qml");

    qmlRegisterType<Server>("io.qt.Backend_Server", 1, 0, "Backend_Server");
    qmlRegisterType<Wrapper_Device>("io.qt.Backend_Device", 1, 0, "Backend_Device");
    qmlRegisterType<Wrapper_Raw>("io.qt.Raw", 1, 0, "Raw");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
