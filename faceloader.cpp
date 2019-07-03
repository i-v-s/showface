#include <memory>
#include <algorithm>
#include <QFile>
#include <QString>
#include <QHttpMultiPart>
#include <QNetworkRequest>
#include <QFileInfo>
#include "faceloader.h"
#include <QDebug>


auto FaceLoader::upload(const QUrl &url)
{
    const static QStringList extensions = {"gif", "png"};
    using namespace std;
    auto localFileName = url.toLocalFile();
    auto file = make_unique<QFile>(localFileName);
    if (!file->open(QIODevice::ReadOnly)) {
        return make_unique<Result>(QString("%1: file not found").arg(localFileName));
    }
    QNetworkRequest request(QUrl("https://backend.facecloud.tevian.ru/api/v1/detect?demographics=true"));
    auto extension = QFileInfo(localFileName).suffix().toLower();
    QString type = "image/jpeg";
    if (extensions.contains(extension))
        type = "image/" + extension;
    request.setRawHeader("Content-Type", type.toLatin1());
    request.setRawHeader("Content-Transfer-Encoding","binary");
    request.setRawHeader("accept", "application/json");
    request.setRawHeader("Authorization", ("Bearer " + this->m_accessToken).toLocal8Bit());

    return make_unique<Result>(unique_ptr<QNetworkReply>(m_networkAccessManager->post(request, file.get())), move(file));
}

int FaceLoader::resultsCount(QQmlListProperty<Result> *list)
{
    return static_cast<int>(reinterpret_cast<FaceLoader*>(list->data)->m_results.size());
}

Result *FaceLoader::result(QQmlListProperty<Result> *list, int i)
{
    return reinterpret_cast<FaceLoader*>(list->data)->result(i);
}

Result *FaceLoader::result(int i)
{
    return m_results[static_cast<size_t>(i)].get();
}

FaceLoader::FaceLoader(QObject *parent) : QObject(parent)
{
    m_networkAccessManager = new QNetworkAccessManager(this);
    // TODO: Hide token
    m_accessToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiJkOWVmMzIzZS1lYTlhLTRhMGUtYjY0Mi1hMjA1NDM1NDZkMWIiLCJzdWIiOjMxLCJpYXQiOjE1NjE2NjM3NDAsIm5iZiI6MTU2MTY2Mzc0MCwidHlwZSI6ImFjY2VzcyIsImZyZXNoIjpmYWxzZX0.9Tbtfxf0Rwa3R0H_3jgP6jSC8ukLcxKFhsJLEOF9nCs";
}

QList<QUrl> FaceLoader::files() const
{
    return m_files;
}

float FaceLoader::percentCompleted() const
{
    return m_percentCompleted;
}

QQmlListProperty<Result> FaceLoader::results()
{
    return QQmlListProperty<Result>(this, this, &FaceLoader::resultsCount, &FaceLoader::result);
}

void FaceLoader::setFiles(const QList<QUrl> &files)
{
    using namespace std;
    if (m_files == files)
        return;
    m_results.resize(static_cast<size_t>(files.size()));
    transform(files.begin(), files.end(), m_results.begin(), [this](QUrl url){return upload(url);});
    for(auto &result: m_results)
        connect(result.get(), &Result::percentCompletedChanged, this, &FaceLoader::resultPercentCompletedChanged);
    m_files = files;
    emit resultsChanged(this->results());
    emit filesChanged(m_files);
}

void FaceLoader::resultPercentCompletedChanged(float)
{
    float sum = std::accumulate(m_results.begin(), m_results.end(), 0.0f, [](float a, const auto &r){return a + r->percentCompleted();});
    m_percentCompleted = sum / m_results.size();
    emit percentCompletedChanged(m_percentCompleted);
}
