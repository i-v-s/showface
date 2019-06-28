#include <QFile>
#include <QString>
#include <QHttpMultiPart>
#include <QNetworkRequest>
#include <QFileInfo>
#include "loader.h"


void Loader::upload(const QUrl &url)
{
    auto *file = new QFile(url.toLocalFile());
    if (!file->open(QIODevice::ReadOnly)) {
        // emit imageUploaded(999, "Image not found");
        return;
    }
    auto *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));

    imagePart.setBodyDevice(file);
    file->setParent(multiPart);
    multiPart->append(imagePart);

    QNetworkRequest request(QUrl("https://backend.facecloud.tevian.ru/api/v1/detect?demographics=true&attributes=true&landmarks=false"));
    request.setRawHeader("accept", "application/json");
    request.setRawHeader("Authorization", ("Bearer " + this->m_accessToken).toLocal8Bit());

    m_networkReply = m_networkAccessManager->post(request, multiPart);
    multiPart->setParent(m_networkReply);

    connect(m_networkReply, SIGNAL(finished()), this, SLOT(uploadFinished()));
}

Loader::Loader(QObject *parent) : QObject(parent)
{
    m_networkAccessManager = new QNetworkAccessManager(this);
    m_accessToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiJkOWVmMzIzZS1lYTlhLTRhMGUtYjY0Mi1hMjA1NDM1NDZkMWIiLCJzdWIiOjMxLCJpYXQiOjE1NjE2NjM3NDAsIm5iZiI6MTU2MTY2Mzc0MCwidHlwZSI6ImFjY2VzcyIsImZyZXNoIjpmYWxzZX0.9Tbtfxf0Rwa3R0H_3jgP6jSC8ukLcxKFhsJLEOF9nCs";
}

QList<QUrl> Loader::files() const
{
    return m_files;
}

void Loader::setFiles(QList<QUrl> files)
{
    if (m_files == files)
        return;
    upload(files[0]);

    m_files = files;
    emit filesChanged(m_files);
}

void Loader::uploadFinished()
{
    if(m_networkReply->error() == QNetworkReply::NoError)
    {
        QByteArray response = m_networkReply->readAll();
        QString s = response;
        // do something with the data...
    }
    else // handle error
    {
        // qDebug(m_networkReply->errorString());
    }
}
