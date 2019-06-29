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
            auto nameList = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
            std::transform(nameList.begin(), nameList.end(), nameList.begin(), [&dir](auto name){return dir.filePath(name);});
            auto list = prepareNames(nameList);
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
    int readyCount = 0;
    for (int i = 0; i < urls.length(); i++) {
        auto result = loader.result(i);
        QObject::connect(result, &Result::readyChanged, [i, result, &readyCount, &urls, &app](bool ready) {
            if (ready) {
                readyCount++;
                auto jsonFileName = urls[i].toLocalFile() + ".json";
                QFile jsonFile(jsonFileName);
                jsonFile.open(QFile::WriteOnly);
                jsonFile.write(QJsonDocument::fromVariant(result->faces()).toJson());
                qInfo() << jsonFileName << "processed. Completed" << readyCount << "out of" << urls.size();
                if (readyCount == urls.size())
                    app.exit();
            }
        });
    }
    return app.exec();
}
