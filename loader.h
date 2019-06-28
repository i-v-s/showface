#ifndef LOADER_H
#define LOADER_H

#include <QObject>
#include <QList>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class Loader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QUrl> files READ files WRITE setFiles NOTIFY filesChanged)
    QList<QUrl> m_files;
    void upload(const QUrl &url);
    QNetworkAccessManager *m_networkAccessManager;
    QNetworkReply *m_networkReply;
    QString m_accessToken;
public:
    explicit Loader(QObject *parent = nullptr);
    QList<QUrl> files() const;

signals:
    void filesChanged(QList<QUrl> files);

public slots:
    void setFiles(QList<QUrl> files);

private slots:
    void uploadFinished();
};

#endif // LOADER_H
