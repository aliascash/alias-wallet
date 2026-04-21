// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2009 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef CLIENTMODEL_H
#define CLIENTMODEL_H

#include <QObject>
#include <QDateTime>

#include <atomic>

enum class BlockSource {
    NONE,
    REINDEX,
    DISK,
    NETWORK
};

enum NumConnections {
    CONNECTIONS_NONE = 0,
    CONNECTIONS_IN   = (1U << 0),
    CONNECTIONS_OUT  = (1U << 1),
    CONNECTIONS_ALL  = (CONNECTIONS_IN | CONNECTIONS_OUT),
};

class OptionsModel;
class PeerTableModel;
class AddressTableModel;
class TransactionTableModel;
class CWallet;

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

struct CoreInfoModel {
    int numBlocks;
    int numBlocksOfPeers;
    bool isInitialBlockDownload;
    int64_t lastBlockTime;
};
Q_DECLARE_METATYPE(CoreInfoModel);

class ClientModel : public QObject
{
    Q_OBJECT

public:
    explicit ClientModel(OptionsModel *optionsModel, QObject *parent = nullptr);
    ~ClientModel();

    OptionsModel *getOptionsModel();
    PeerTableModel *getPeerTableModel();

    int getNumConnections(unsigned int flags = CONNECTIONS_ALL) const;
    int getNumBlocks() const;
    int getNumBlocksOfPeers() const;

    quint64 getTotalBytesRecv() const;
    quint64 getTotalBytesSent() const;

    QDateTime getLastBlockDate() const;

    bool isTestNet() const;
    int getClientMode() const;
    bool inInitialBlockDownload() const;
    bool isImporting() const;
    QString getStatusBarWarnings() const;

    QString formatFullVersion() const;
    QString formatBuildDate() const;
    QString clientName() const;
    QString formatClientStartupTime() const;

private:
    OptionsModel *optionsModel;
    PeerTableModel *peerTableModel{nullptr};

    CoreInfoModel coreInfo{};
    CoreInfoModel lastPublishedCoreInfo{};

    QTimer *pollTimer{nullptr};

    void subscribeToCoreSignals();
    void unsubscribeFromCoreSignals();

Q_SIGNALS:
    void numConnectionsChanged(int count);
    void numBlocksChanged(int count, int countOfPeers);
    void bytesChanged(quint64 totalBytesIn, quint64 totalBytesOut);
    void alertsChanged(const QString &warnings);
    void error(const QString &title, const QString &message, bool modal);

public Q_SLOTS:
    void updateTimer();
    void updateFromCore(const CoreInfoModel &coreInfo);
    void updateNumConnections(int numConnections);
    void updateAlert(const QString &hash, int status);
};

#endif // CLIENTMODEL_H
