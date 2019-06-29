#include <memory>
#include <algorithm>
#include <QFile>
#include <QString>
#include <QHttpMultiPart>
#include <QNetworkRequest>
#include <QFileInfo>
#include "loader.h"
#include <QDebug>


auto Loader::upload(const QUrl &url)
{
    using namespace std;
    auto file = make_unique<QFile>(url.toLocalFile());
    if (!file->open(QIODevice::ReadOnly)) {
        // TODO: emit imageUploaded(999, "Image not found");
        return make_unique<Result>();
    }
    QNetworkRequest request(QUrl("https://backend.facecloud.tevian.ru/api/v1/detect?demographics=true"));
    request.setRawHeader("Content-Type", "image/jpeg");
    request.setRawHeader("Content-Transfer-Encoding","binary");
    request.setRawHeader("accept", "application/json");
    request.setRawHeader("Authorization", ("Bearer " + this->m_accessToken).toLocal8Bit());

    return make_unique<Result>(unique_ptr<QNetworkReply>(m_networkAccessManager->post(request, file.get())), move(file));
}

int Loader::resultsCount(QQmlListProperty<Result> *list)
{
    return static_cast<int>(reinterpret_cast<Loader*>(list->data)->m_results.size());
}

Result *Loader::result(QQmlListProperty<Result> *list, int i)
{
    return reinterpret_cast<Loader*>(list->data)->result(i);
}

Result *Loader::result(int i)
{
    return m_results[static_cast<size_t>(i)].get();
}

Loader::Loader(QObject *parent) : QObject(parent)
{
    m_networkAccessManager = new QNetworkAccessManager(this);
    // TODO: Hide token
    m_accessToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiJkOWVmMzIzZS1lYTlhLTRhMGUtYjY0Mi1hMjA1NDM1NDZkMWIiLCJzdWIiOjMxLCJpYXQiOjE1NjE2NjM3NDAsIm5iZiI6MTU2MTY2Mzc0MCwidHlwZSI6ImFjY2VzcyIsImZyZXNoIjpmYWxzZX0.9Tbtfxf0Rwa3R0H_3jgP6jSC8ukLcxKFhsJLEOF9nCs";
}

QList<QUrl> Loader::files() const
{
    return m_files;
}

float Loader::percentCompleted() const
{
    return m_percentCompleted;
}

QQmlListProperty<Result> Loader::results()
{
    return QQmlListProperty<Result>(this, this, &Loader::resultsCount, &Loader::result);
}

void Loader::setFiles(const QList<QUrl> &files)
{
    using namespace std;
    if (m_files == files)
        return;
    m_results.resize(static_cast<size_t>(files.size()));
    transform(files.begin(), files.end(), m_results.begin(), [this](QUrl url){return upload(url);});
    for(auto &result: m_results)
        connect(result.get(), &Result::percentCompletedChanged, this, &Loader::resultPercentCompletedChanged);
    m_files = files;
    emit resultsChanged(this->results());
    emit filesChanged(m_files);
}

void Loader::resultPercentCompletedChanged(float)
{
    float sum = std::accumulate(m_results.begin(), m_results.end(), 0.0f, [](float a, const auto &r){return a + r->percentCompleted();});
    m_percentCompleted = sum / m_results.size();
    emit percentCompletedChanged(m_percentCompleted);
}
