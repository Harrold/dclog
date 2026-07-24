#ifndef CONTAINERMODEL_H
#define CONTAINERMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QVector>

struct ContainerInfo
{
    QString id;
    QString name;
    QString image;
    QString status;
};

class ContainerListModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ContainerListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole) const override;

    void refresh(const QVector<ContainerInfo> &latest);

    QString idAt(int row) const;

private:
    QVector<ContainerInfo> m_containers;
};

#endif // CONTAINERMODEL_H
