#include "containermodel.h"

#include <algorithm>

namespace {
constexpr int ColumnName = 0;
constexpr int ColumnImage = 1;
constexpr int ColumnStatus = 2;
constexpr int ColumnCount = 3;
}

ContainerListModel::ContainerListModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int ContainerListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_containers.size();
}

int ContainerListModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return ColumnCount;
}

QVariant ContainerListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_containers.size())
        return {};

    if (role != Qt::DisplayRole)
        return {};

    const ContainerInfo &c = m_containers.at(index.row());
    switch (index.column()) {
    case ColumnName:
        return c.name;
    case ColumnImage:
        return c.image;
    case ColumnStatus:
        return c.status;
    default:
        return {};
    }
}

QVariant ContainerListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QAbstractTableModel::headerData(section, orientation, role);

    switch (section) {
    case ColumnName:
        return tr("Name");
    case ColumnImage:
        return tr("Image");
    case ColumnStatus:
        return tr("Status");
    default:
        return {};
    }
}

void ContainerListModel::refresh(const QVector<ContainerInfo> &latest)
{
    // Remove rows whose container is no longer running.
    for (int row = m_containers.size() - 1; row >= 0; --row) {
        const QString &id = m_containers.at(row).id;
        const bool stillPresent = std::any_of(latest.begin(), latest.end(),
                                               [&id](const ContainerInfo &c) { return c.id == id; });
        if (!stillPresent) {
            beginRemoveRows(QModelIndex(), row, row);
            m_containers.removeAt(row);
            endRemoveRows();
        }
    }

    // Update existing rows and append new ones.
    for (const ContainerInfo &c : latest) {
        int row = -1;
        for (int i = 0; i < m_containers.size(); ++i) {
            if (m_containers.at(i).id == c.id) {
                row = i;
                break;
            }
        }

        if (row == -1) {
            const int newRow = m_containers.size();
            beginInsertRows(QModelIndex(), newRow, newRow);
            m_containers.append(c);
            endInsertRows();
        } else if (m_containers.at(row).name != c.name
                   || m_containers.at(row).image != c.image
                   || m_containers.at(row).status != c.status) {
            m_containers[row] = c;
            emit dataChanged(index(row, 0), index(row, ColumnCount - 1));
        }
    }
}

QString ContainerListModel::idAt(int row) const
{
    if (row < 0 || row >= m_containers.size())
        return {};
    return m_containers.at(row).id;
}
