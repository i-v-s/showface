#include <QJsonDocument>
#include <QJsonArray>
#include "result.h"

void Result::uploadFinished()
{
    m_device->close();
    m_device = nullptr;
    if(m_networkReply->error() == QNetworkReply::NoError)
    {
        auto document = QJsonDocument::fromJson(m_networkReply->readAll());
        auto array = document["data"].toArray();
        m_faces.clear();
        for(auto obj: array)
            m_faces.append(QVariant(obj));
        m_ready = true;
        m_percentCompleted = 100.0f;
        emit facesChanged(m_faces);
        emit readyChanged(m_ready);
        emit percentCompletedChanged(m_percentCompleted);
    }
    else // handle error
    {
        QByteArray response = m_networkReply->readAll();
        QString s = response;
        qDebug() << m_networkReply->errorString();

    }
    m_networkReply = nullptr;
}

void Result::uploadProgress(qint64 bytesSent, quint64 bytesTotal)
{
    m_percentCompleted = float(bytesSent) / float(bytesTotal) * 85.0f;
    emit percentCompletedChanged(m_percentCompleted);
}

Result::Result(QObject *parent) : QObject(parent), m_ready(false), m_percentCompleted(0.0f)
{

}

Result::Result(std::unique_ptr<QNetworkReply> networkReply, std::unique_ptr<QIODevice> file, QObject *parent) :
    QObject(parent), m_device(std::move(file)), m_networkReply(std::move(networkReply))
{
    connect(m_networkReply.get(), &QNetworkReply::finished, this, &Result::uploadFinished);
    connect(m_networkReply.get(), &QNetworkReply::uploadProgress, this, &Result::uploadProgress);
}

QList<QVariant> Result::faces() const
{
    return m_faces;
}

bool Result::ready() const
{
    return m_ready;
}

float Result::percentCompleted() const
{
    return m_percentCompleted;
}
