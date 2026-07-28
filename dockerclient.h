#ifndef DOCKERCLIENT_H
#define DOCKERCLIENT_H

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QVector>

#include "containermodel.h"

class QProcess;

class DockerClient : public QObject
{
    Q_OBJECT

public:
    explicit DockerClient(QObject *parent = nullptr);
    ~DockerClient() override;

public slots:
    void pollContainers();
    void streamLogs(const QString &containerId);
    void stopLogs();

signals:
    void containersUpdated(const QVector<ContainerInfo> &containers);
    void logLine(const QString &text);

private slots:
    void handlePollFinished();
    void handleLogOutput();
    void flushLogBuffer();

private:
    QProcess *m_pollProcess = nullptr;
    QProcess *m_logProcess = nullptr;
    QByteArray m_logBuffer;
};

#endif // DOCKERCLIENT_H
