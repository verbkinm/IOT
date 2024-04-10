#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>

#include "client.h"
#include "producer.h"
#include "qthread.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("VMS");
    app.setApplicationName("IOTV_Client");
    app.setApplicationVersion("0.16.4");
    app.setWindowIcon(QIcon(":/img/logo.png"));

    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:/");

    QThread *th = new QThread;
    Client *client = new Client(th);

    th->start();

//    Client client;
    engine.rootContext()->setContextProperty("client", client);

    qmlRegisterType<Producer>("Producer", 1, 0, "Producer");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](const QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
