#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "result.h"

void Result::uploadFinished()
{
    m_device->close();
    m_device = nullptr;
    if(m_networkReply->error() == QNetworkReply::NoError)
    {
        auto document = QJsonDocument::fromJson(m_networkReply->readAll());
        auto array = document.object()["data"].toArray();
        m_faces.clear();
        for(auto obj: array)
            m_faces.append(QVariant(obj));
        m_ready = true;
        m_percentCompleted = 100.0f;
        emit facesChanged(m_faces);
        emit readyChanged(m_ready);
        emit percentCompletedChanged(m_percentCompleted);
    }
    m_networkReply = nullptr;
}

void Result::uploadProgress(qint64 bytesSent, quint64 bytesTotal)
{
    m_percentCompleted = bytesTotal ? float(bytesSent) / float(bytesTotal) * 85.0f : 0.0f;
    emit percentCompletedChanged(m_percentCompleted);
}

void Result::networkError(QNetworkReply::NetworkError)
{
    m_errorMessage = m_networkReply->errorString();
    emit errorMessageChanged(m_errorMessage);
}

Result::Result(QObject *parent) :
    QObject(parent), m_ready(false), m_percentCompleted(0.0f)
{

}

Result::Result(const QString &errorMessage, QObject *parent) :
    QObject(parent), m_ready(false), m_percentCompleted(0.0f), m_errorMessage(errorMessage)
{

}

Result::Result(std::unique_ptr<QNetworkReply> networkReply, std::unique_ptr<QIODevice> file, QObject *parent) :
    QObject(parent), m_device(std::move(file)), m_networkReply(std::move(networkReply))
{
    connect(m_networkReply.get(), &QNetworkReply::finished, this, &Result::uploadFinished);
    connect(m_networkReply.get(), &QNetworkReply::uploadProgress, this, &Result::uploadProgress);
    connect(m_networkReply.get(), static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &Result::networkError);
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

QVariant Result::errorMessage() const
{
    return m_errorMessage;
}
