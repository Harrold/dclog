#ifndef DOCKERCLIENT_H
#define DOCKERCLIENT_H

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

private:
    QProcess *m_pollProcess = nullptr;
    QProcess *m_logProcess = nullptr;
};

#endif // DOCKERCLIENT_H
