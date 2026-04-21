// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2011 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef BITCOIN_QT_PEERTABLEMODEL_H
#define BITCOIN_QT_PEERTABLEMODEL_H

#include "main.h"
#include "net.h"

#include <QAbstractTableModel>
#include <QList>
#include <QModelIndex>
#include <QStringList>
#include <QVariant>

class ClientModel;

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

struct CNodeCombinedStats {
    CNodeStats nodeStats;
    CNodeStateStats nodeStateStats;
    bool fNodeStateStatsAvailable;
};
Q_DECLARE_METATYPE(CNodeCombinedStats*)

class PeerTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit PeerTableModel(ClientModel *parent = nullptr);
    ~PeerTableModel();
    const CNodeCombinedStats *getNodeStats(int idx);
    int getRowByNodeId(NodeId nodeid);
    void startAutoRefresh();
    void stopAutoRefresh();

    enum ColumnIndex {
        NetNodeId = 0,
        Age,
        Address,
        Direction,
        Network,
        Ping,
        Sent,
        Received,
        Subversion
    };

    enum {
        StatsRole = Qt::UserRole,
    };

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

public Q_SLOTS:
    void refresh();

private:
    QList<CNodeCombinedStats> m_peers_data;
    ClientModel *clientModel;
    const QStringList columns{
        tr("Peer"),
        tr("Age"),
        tr("Address"),
        tr("Direction"),
        tr("Network"),
        tr("Ping"),
        tr("Sent"),
        tr("Received"),
        tr("User Agent")};
    QTimer* timer{nullptr};
};

#endif // BITCOIN_QT_PEERTABLEMODEL_H
