// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// Copyright (c) 2018-2022 The Bitcoin Core developers
// SPDX-License-Identifier: MIT

#ifndef ALIAS_INTERFACES_NODE_H
#define ALIAS_INTERFACES_NODE_H

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

class CNodeStats;
class proxyType;
class uint256;

namespace interfaces {

class Chain;
class Handler;
class Wallet;
class WalletLoader;

//! Top-level interface for a node (aliascoind process)
class Node {
public:
    virtual ~Node() = default;

    //! Init logging
    virtual void initLogging() = 0;

    //! Init parameter interaction
    virtual void initParameterInteraction() = 0;

    //! Get warnings
    virtual std::string getWarnings() = 0;

    //! Get log flags
    virtual uint32_t getLogCategories() = 0;

    //! Initialize app dependencies
    virtual bool baseInitialize() = 0;

    //! Start node
    virtual bool appInitMain() = 0;

    //! Stop node
    virtual void appShutdown() = 0;

    //! Start shutdown
    virtual void startShutdown() = 0;

    //! Return whether shutdown was requested
    virtual bool shutdownRequested() = 0;

    //! Get network active state
    virtual bool getNetworkActive() const = 0;

    //! Set network active state
    virtual void setNetworkActive(bool active) = 0;

    //! Get number of connections
    virtual size_t getNodeCount() const = 0;

    //! Get peer info
    virtual bool getNodesStats(std::vector<CNodeStats>& stats) const = 0;

    //! Disconnect by node id
    virtual bool disconnectByAddress(const std::string& address) = 0;

    //! Disconnect by node id
    virtual bool disconnectById(int64_t id) = 0;

    //! Get number of blocks
    virtual int getNumBlocks() const = 0;

    //! Get best block hash
    virtual uint256 getBestBlockHash() const = 0;

    //! Get last block time
    virtual int64_t getLastBlockTime() const = 0;

    //! Get verification progress
    virtual double getVerificationProgress() const = 0;

    //! Is initial block download
    virtual bool isInitialBlockDownload() const = 0;

    //! Get reindex status
    virtual bool getReindex() const = 0;

    //! Get importing status
    virtual bool getImporting() const = 0;

    //! Get network name
    virtual std::string getNetworkName() const = 0;

    //! Get proxy
    virtual bool getProxy(std::string& proxy_info) const = 0;

    //! Get mempool size
    virtual size_t getMempoolSize() const = 0;

    //! Get mempool dynamic usage
    virtual size_t getMempoolDynamicUsage() const = 0;

    //! Notifications interface for GUI
    class Notifications {
    public:
        virtual ~Notifications() = default;
        virtual void initMessage(const std::string& message) {}
        virtual void showProgress(const std::string& title, int progress, bool resume_possible) {}
        virtual void numConnectionsChanged(int num_connections) {}
        virtual void numBlocksChanged(int num_blocks, int64_t block_time, double verification_progress) {}
        virtual void alertNotify(const std::string& message) {}
        virtual void bannedListChanged() {}
    };

    //! Register handler for notifications
    virtual std::unique_ptr<Handler> handleNotifications(std::shared_ptr<Notifications> notifications) = 0;

    //! Get wallet loader
    virtual WalletLoader& walletLoader() = 0;

    //! Get chain interface
    virtual Chain& chain() = 0;
};

//! Return implementation of Node interface
std::unique_ptr<Node> MakeNode();

} // namespace interfaces

#endif // ALIAS_INTERFACES_NODE_H
