#ifndef RESULT_H
#define RESULT_H

#include <memory>

#include <QObject>
#include <QIODevice>
#include <QNetworkReply>

class Result : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QVariant> faces READ faces NOTIFY facesChanged)
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(float percentCompleted READ percentCompleted NOTIFY percentCompletedChanged)
    Q_PROPERTY(QVariant errorMessage READ errorMessage NOTIFY errorMessageChanged)

    std::unique_ptr<QIODevice> m_device;
    std::unique_ptr<QNetworkReply> m_networkReply;
    QList<QVariant> m_faces;
    bool m_ready;
    float m_percentCompleted;
    QVariant m_errorMessage;

public:
    explicit Result(QObject *parent = nullptr);
    Result(const QString &errorMessage, QObject *parent = nullptr);
    Result(std::unique_ptr<QNetworkReply> networkReply, std::unique_ptr<QIODevice> m_device, QObject *parent = nullptr);
    QList<QVariant> faces() const;
    bool ready() const;
    float percentCompleted() const;
    QVariant errorMessage() const;

signals:
    void facesChanged(QList<QVariant> faces);
    void readyChanged(bool ready);
    void percentCompletedChanged(float percentCompleted);
    void errorMessageChanged(QVariant errorMessage);

private slots:
    void uploadFinished();
    void uploadProgress(qint64 bytesSent, quint64 bytesTotal);
    void networkError(QNetworkReply::NetworkError);
};

#endif // RESULT_H
