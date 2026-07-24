#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dockerclient.h"

#include <QScrollBar>
#include <QTimer>

namespace {
constexpr int PollIntervalMs = 2000;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
    , m_model(new ContainerListModel(this))
    , m_dockerClient(new DockerClient(this))
    , m_pollTimer(new QTimer(this))
{
    ui->setupUi(this);

    ui->containerTable->setModel(m_model);
    ui->containerTable->horizontalHeader()->setStretchLastSection(true);
    ui->containerTable->verticalHeader()->setVisible(false);

    QFont monoFont(QStringLiteral("Monospace"));
    monoFont.setStyleHint(QFont::TypeWriter);
    ui->logView->setFont(monoFont);

    connect(ui->containerTable, &QTableView::clicked, this, &MainWindow::onContainerClicked);
    connect(m_dockerClient, &DockerClient::containersUpdated, this, &MainWindow::onContainersUpdated);
    connect(m_dockerClient, &DockerClient::logLine, this, &MainWindow::onLogLine);

    connect(m_pollTimer, &QTimer::timeout, m_dockerClient, &DockerClient::pollContainers);
    m_pollTimer->start(PollIntervalMs);
    m_dockerClient->pollContainers();
}

MainWindow::~MainWindow() = default;

void MainWindow::onContainerClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    const QString id = m_model->idAt(index.row());
    if (id.isEmpty() || id == m_currentContainerId)
        return;

    m_currentContainerId = id;
    ui->logView->clear();
    m_dockerClient->streamLogs(id);
}

void MainWindow::onContainersUpdated(const QVector<ContainerInfo> &containers)
{
    m_model->refresh(containers);
}

void MainWindow::onLogLine(const QString &text)
{
    QScrollBar *scrollBar = ui->logView->verticalScrollBar();
    const bool atBottom = scrollBar->value() >= scrollBar->maximum() - 4;

    ui->logView->moveCursor(QTextCursor::End);
    ui->logView->insertPlainText(text);

    if (atBottom)
        scrollBar->setValue(scrollBar->maximum());
}
