#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "loader.h"
#include "result.h"

int main(int argc, char *argv[])
{
    // TODO: auto s = QSslSocket::supportsSsl();
    //QString b = QSslSocket::sslLibraryBuildVersionString();
    //QString v = QSslSocket::sslLibraryVersionString();
    //printf(b.toLocal8Bit().data());
    //printf(v.toLocal8Bit().data());

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    app.setOrganizationName("Tevian");
    app.setOrganizationDomain("tevian.ru");
    app.setApplicationName("Show Face Application");
    qmlRegisterType<Loader>("showface.backend", 1, 0, "Loader");
    qmlRegisterType<Result>("showface.backend", 1, 0, "Result");

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
