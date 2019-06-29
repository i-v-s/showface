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

    std::unique_ptr<QIODevice> m_device;
    std::unique_ptr<QNetworkReply> m_networkReply;
    QList<QVariant> m_faces;
    bool m_ready;
    float m_percentCompleted;

public:
    explicit Result(QObject *parent = nullptr);
    Result(std::unique_ptr<QNetworkReply> networkReply, std::unique_ptr<QIODevice> m_device, QObject *parent = nullptr);
    QList<QVariant> faces() const;
    bool ready() const;
    float percentCompleted() const;

signals:
    void facesChanged(QList<QVariant> faces);
    void readyChanged(bool ready);
    void percentCompletedChanged(float percentCompleted);

private slots:
    void uploadFinished();
    void uploadProgress(qint64 bytesSent, quint64 bytesTotal);
};

#endif // RESULT_H
