#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextStream>
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
        } else if (fileInfo.isFile() && extensions.contains(fileInfo.suffix().toLower()))
            result.append(QUrl::fromLocalFile(name));
        else QTextStream(stdout) << QString("Warning: %1 is not an image").arg(name) << endl;
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
    auto urls = prepareNames(parser.positionalArguments());
    if (urls.empty())
        return 0;
    Loader loader;
    urls[0] = QUrl::fromLocalFile("aaa");
    loader.setFiles(urls);
    int processedCount = 0;
    for (int i = 0; i < urls.length(); i++) {
        auto result = loader.result(i);
        auto handleError = [&, i](const QVariant &errorMessage){
            if (errorMessage.isValid()) {
                processedCount++;
                QTextStream(stdout) << QString("%1/%2: %3 [%4]").arg(processedCount).arg(urls.size()).arg(errorMessage.toString()).arg(urls[i].toLocalFile()) << endl;
                if (processedCount == urls.size())
                    app.exit();
                return true;
            } else
                return false;
        };
        if (!handleError(result->errorMessage())) {
            QObject::connect(result, &Result::errorMessageChanged, handleError);
            QObject::connect(result, &Result::readyChanged, [&, i, result](bool ready) {
                if (ready) {
                    processedCount++;
                    auto jsonFileName = urls[i].toLocalFile() + ".json";
                    QFile jsonFile(jsonFileName);
                    jsonFile.open(QFile::WriteOnly);
                    jsonFile.write(QJsonDocument::fromVariant(result->faces()).toJson());
                    QTextStream(stdout) << QString("%1/%2: Writed %3").arg(processedCount).arg(urls.size()).arg(jsonFileName) << endl;
                    if (processedCount == urls.size())
                        app.exit();
                }
            });
        }
    }
    return app.exec();
}
