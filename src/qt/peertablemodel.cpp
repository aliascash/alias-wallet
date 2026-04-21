// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2011 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#include "peertablemodel.h"

#include "clientmodel.h"
#include "guiconstants.h"
#include "guiutil.h"

#include "net.h"
#include "sync.h"

#include <QList>
#include <QTimer>

PeerTableModel::PeerTableModel(ClientModel *parent) :
    QAbstractTableModel(parent),
    clientModel(parent)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &PeerTableModel::refresh);
    timer->setInterval(MODEL_UPDATE_DELAY);

    refresh();
}

PeerTableModel::~PeerTableModel() = default;

const CNodeCombinedStats *PeerTableModel::getNodeStats(int idx)
{
    if (idx >= 0 && idx < m_peers_data.size()) {
        return &m_peers_data[idx];
    }
    return nullptr;
}

int PeerTableModel::getRowByNodeId(NodeId nodeid)
{
    for (int i = 0; i < m_peers_data.size(); ++i) {
        if (m_peers_data.at(i).nodeStats.nodeid == nodeid) {
            return i;
        }
    }
    return -1;
}

void PeerTableModel::startAutoRefresh()
{
    timer->start();
}

void PeerTableModel::stopAutoRefresh()
{
    timer->stop();
}

int PeerTableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_peers_data.size();
}

int PeerTableModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return columns.length();
}

QVariant PeerTableModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    CNodeCombinedStats *rec = static_cast<CNodeCombinedStats*>(index.internalPointer());

    const auto column = static_cast<ColumnIndex>(index.column());
    if (role == Qt::DisplayRole) {
        switch (column) {
        case NetNodeId:
            return (qint64)rec->nodeStats.nodeid;
        case Age:
            return GUIUtil::FormatPeerAge(rec->nodeStats.nTimeConnected);
        case Address:
            return QString::fromStdString(rec->nodeStats.addrName);
        case Direction:
            return QString(rec->nodeStats.fInbound ? tr("Inbound") : tr("Outbound"));
        case Network:
            return GUIUtil::NetworkToQString(rec->nodeStats.m_network);
        case Ping:
            return GUIUtil::formatPingTime(rec->nodeStats.dPingTime);
        case Sent:
            return GUIUtil::formatBytes(rec->nodeStats.nSendBytes);
        case Received:
            return GUIUtil::formatBytes(rec->nodeStats.nRecvBytes);
        case Subversion:
            return QString::fromStdString(rec->nodeStats.strSubVer);
        }
    } else if (role == Qt::TextAlignmentRole) {
        switch (column) {
        case NetNodeId:
        case Age:
            return QVariant(Qt::AlignRight | Qt::AlignVCenter);
        case Address:
            return {};
        case Direction:
        case Network:
            return QVariant(Qt::AlignCenter);
        case Ping:
        case Sent:
        case Received:
            return QVariant(Qt::AlignRight | Qt::AlignVCenter);
        case Subversion:
            return {};
        }
    } else if (role == StatsRole) {
        return QVariant::fromValue(rec);
    }

    return QVariant();
}

QVariant PeerTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole && section < columns.size())
        {
            return columns[section];
        }
    }
    return QVariant();
}

Qt::ItemFlags PeerTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;

    Qt::ItemFlags retval = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return retval;
}

QModelIndex PeerTableModel::index(int row, int column, const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    if (0 <= row && row < rowCount() && 0 <= column && column < columnCount()) {
        return createIndex(row, column, const_cast<CNodeCombinedStats*>(&m_peers_data[row]));
    }

    return QModelIndex();
}

void PeerTableModel::refresh()
{
    QList<CNodeCombinedStats> new_peers_data;

    {
        TRY_LOCK(cs_vNodes, lockNodes);
        if (!lockNodes) {
            return;
        }
        new_peers_data.reserve(vNodes.size());
        for (CNode* pnode : vNodes) {
            CNodeCombinedStats stats;
            stats.nodeStateStats.nMisbehavior = 0;
            stats.nodeStateStats.nSyncHeight = -1;
            stats.fNodeStateStatsAvailable = false;
            pnode->copyStats(stats.nodeStats);
            new_peers_data.append(stats);
        }
    }

    {
        TRY_LOCK(cs_main, lockMain);
        if (lockMain) {
            for (CNodeCombinedStats& stats : new_peers_data)
                stats.fNodeStateStatsAvailable = GetNodeStateStats(stats.nodeStats.nodeid, stats.nodeStateStats);
        }
    }

    for (int i = 0; i < m_peers_data.size();) {
        if (i < new_peers_data.size() && m_peers_data.at(i).nodeStats.nodeid == new_peers_data.at(i).nodeStats.nodeid) {
            ++i;
            continue;
        }
        beginRemoveRows(QModelIndex(), i, i);
        m_peers_data.erase(m_peers_data.begin() + i);
        endRemoveRows();
    }

    if (m_peers_data.size() < new_peers_data.size()) {
        beginInsertRows(QModelIndex(), m_peers_data.size(), new_peers_data.size() - 1);
        m_peers_data.swap(new_peers_data);
        endInsertRows();
    } else {
        m_peers_data.swap(new_peers_data);
    }

    const auto top_left = index(0, 0);
    const auto bottom_right = index(rowCount() - 1, columnCount() - 1);
    Q_EMIT dataChanged(top_left, bottom_right);
    Q_EMIT layoutChanged();
}
