#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include "loader.h"

QList<QUrl> prepareNames(QStringList names)
{
    const static QStringList extensions{"jpg", "jpeg", "png"};
    QList<QUrl> result;
    for(auto name: names) {
        QFileInfo fileInfo(name);
        if (fileInfo.isDir()) {
            QDir dir(name);
            auto list = prepareNames(dir.entryList());
            for (const auto &item: list)
                result.append(item);
        } else if (fileInfo.isFile() && extensions.contains(fileInfo.suffix()))
            result.append(QUrl::fromLocalFile(name));
        else qWarning() << name << " is not an image";
    }
    return result;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("showface-cli");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Showface command line interface");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
    parser.addPositionalArgument("source", QCoreApplication::translate("main", "Source file to copy."));
    parser.process(app);
    const auto urls = prepareNames(parser.positionalArguments());
    Loader loader;
    loader.setFiles(urls);
    int ready = 0;
    for (int i = 0; i < urls.length(); i++) {
        auto result = loader.result(i);
        QObject::connect(result, &Result::readyChanged, [i, result, &ready, &urls, &app](bool r) {
            if (r) {
                ready++;
                auto jsonFileName = urls[i].toLocalFile() + ".json";
                QFile jsonFile(jsonFileName);
                jsonFile.open(QFile::WriteOnly);
                jsonFile.write(QJsonDocument::fromVariant(result->faces()).toJson());
                qInfo() << jsonFileName << "processed. Completed" << ready << "out of" << urls.size();
                if (ready == urls.size())
                    app.exit();
            }
        });
    }
    return app.exec();
}
