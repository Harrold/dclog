#include "dockerclient.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QProcess>

#include "logformatter.h"

DockerClient::DockerClient(QObject *parent)
    : QObject(parent)
{
}

DockerClient::~DockerClient()
{
    stopLogs();
}

void DockerClient::pollContainers()
{
    if (m_pollProcess)
        return; // previous poll still in flight; skip this tick

    m_pollProcess = new QProcess(this);
    connect(m_pollProcess, &QProcess::finished, this, &DockerClient::handlePollFinished);
    m_pollProcess->start(QStringLiteral("docker"),
                          {QStringLiteral("ps"), QStringLiteral("--format"), QStringLiteral("{{json .}}")});
}

void DockerClient::handlePollFinished()
{
    const QByteArray output = m_pollProcess->readAllStandardOutput();
    m_pollProcess->deleteLater();
    m_pollProcess = nullptr;

    QVector<ContainerInfo> containers;
    const QList<QByteArray> lines = output.split('\n');
    for (const QByteArray &line : lines) {
        const QByteArray trimmed = line.trimmed();
        if (trimmed.isEmpty())
            continue;

        QJsonParseError error;
        const QJsonDocument doc = QJsonDocument::fromJson(trimmed, &error);
        if (error.error != QJsonParseError::NoError || !doc.isObject())
            continue;

        const QJsonObject obj = doc.object();
        ContainerInfo info;
        info.id = obj.value(QStringLiteral("ID")).toString();
        info.name = obj.value(QStringLiteral("Names")).toString();
        info.image = obj.value(QStringLiteral("Image")).toString();
        info.status = obj.value(QStringLiteral("Status")).toString();
        if (!info.id.isEmpty())
            containers.append(info);
    }

    emit containersUpdated(containers);
}

void DockerClient::streamLogs(const QString &containerId)
{
    stopLogs();
    m_logBuffer.clear();

    m_logProcess = new QProcess(this);
    m_logProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_logProcess, &QProcess::readyRead, this, &DockerClient::handleLogOutput);
    connect(m_logProcess, &QProcess::finished, this, &DockerClient::flushLogBuffer);
    m_logProcess->start(QStringLiteral("docker"),
                         {QStringLiteral("logs"), QStringLiteral("-f"), QStringLiteral("--tail"),
                          QStringLiteral("200"), containerId});
}

void DockerClient::handleLogOutput()
{
    if (!m_logProcess)
        return;

    m_logBuffer.append(m_logProcess->readAll());

    QString formatted;
    int newlineIndex;
    while ((newlineIndex = m_logBuffer.indexOf('\n')) != -1) {
        QByteArray rawLine = m_logBuffer.left(newlineIndex);
        m_logBuffer.remove(0, newlineIndex + 1);
        if (rawLine.endsWith('\r'))
            rawLine.chop(1);

        formatted += LogFormatter::formatLine(rawLine);
        formatted += QLatin1Char('\n');
    }

    if (!formatted.isEmpty())
        emit logLine(formatted);
}

void DockerClient::flushLogBuffer()
{
    if (m_logBuffer.isEmpty())
        return;

    emit logLine(LogFormatter::formatLine(m_logBuffer) + QLatin1Char('\n'));
    m_logBuffer.clear();
}

void DockerClient::stopLogs()
{
    if (!m_logProcess)
        return;

    m_logProcess->disconnect(this);
    m_logProcess->kill();
    m_logProcess->waitForFinished(1000);
    m_logProcess->deleteLater();
    m_logProcess = nullptr;
}
