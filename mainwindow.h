#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <memory>

#include "containermodel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QTimer;
class DockerClient;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onContainerClicked(const QModelIndex &index);
    void onContainersUpdated(const QVector<ContainerInfo> &containers);
    void onLogLine(const QString &text);

private:
    std::unique_ptr<Ui::MainWindow> ui;
    ContainerListModel *m_model;
    DockerClient *m_dockerClient;
    QTimer *m_pollTimer;
    QString m_currentContainerId;
};
#endif // MAINWINDOW_H
