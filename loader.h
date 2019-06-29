#ifndef LOADER_H
#define LOADER_H

#include <QObject>
#include <QList>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QQmlListProperty>

#include "result.h"

class Loader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QUrl> files READ files WRITE setFiles NOTIFY filesChanged)
    Q_PROPERTY(int percentCompleted READ percentCompleted NOTIFY percentCompletedChanged)
    Q_PROPERTY(QQmlListProperty<Result> results READ results NOTIFY resultsChanged)
    Q_PROPERTY(float percentCompleted READ percentCompleted NOTIFY percentCompletedChanged)

private:
    QList<QUrl> m_files;
    auto upload(const QUrl &url);
    QNetworkAccessManager *m_networkAccessManager;
    QNetworkReply *m_networkReply;
    QString m_accessToken;
    std::vector<std::unique_ptr<Result>> m_results;
    float m_percentCompleted;

    static int resultsCount(QQmlListProperty<Result>* list);
    static Result* result(QQmlListProperty<Result>*list, int i);

public:
    explicit Loader(QObject *parent = nullptr);
    QList<QUrl> files() const;
    float percentCompleted() const;
    QQmlListProperty<Result> results();
    Result* result(int i);

signals:
    void filesChanged(QList<QUrl> files);
    void percentCompletedChanged(float percentCompleted);

    void resultsChanged(QQmlListProperty<Result> results);

public slots:
    void setFiles(const QList<QUrl> &files);

private slots:
    void resultPercentCompletedChanged(float);
};

#endif // LOADER_H
