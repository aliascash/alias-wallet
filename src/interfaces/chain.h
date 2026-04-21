// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// Copyright (c) 2018-2022 The Bitcoin Core developers
// SPDX-License-Identifier: MIT

#ifndef ALIAS_INTERFACES_CHAIN_H
#define ALIAS_INTERFACES_CHAIN_H

#include <optional>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

class CBlock;
class CBlockIndex;
class CTransaction;
class COutPoint;
class CTxOut;
class uint256;

namespace interfaces {

class Handler;
class Wallet;

//! Interface giving clients (wallet processes, maybe other tools) read-only access
//! to the chain state, chain notifications, and the ability to relay transactions.
class Chain {
public:
    virtual ~Chain() = default;

    //! Get current chain height
    virtual int getHeight() const = 0;

    //! Get block hash at height
    virtual std::optional<uint256> getBlockHash(int height) const = 0;

    //! Check if block is in main chain
    virtual bool haveBlockOnDisk(int height) const = 0;

    //! Get locator for a given block
    virtual std::vector<uint256> getBlockLocator(const uint256& block_hash) const = 0;

    //! Check if transaction is in mempool
    virtual bool isInMempool(const uint256& txid) const = 0;

    //! Check if transaction has confirmations
    virtual bool hasConfirmedTx(const uint256& txid) const = 0;

    //! Relay a transaction
    virtual void relayTransaction(const uint256& txid) = 0;

    //! Get estimated fee for target
    virtual int64_t estimateFee(int target_blocks) const = 0;

    //! Check if chain is syncing
    virtual bool isInitialBlockDownload() const = 0;

    //! Broadcast transaction
    virtual bool broadcastTransaction(const CTransaction& tx, std::string& err_string, int64_t max_fee, bool relay) = 0;

    //! Get unspent output
    virtual std::optional<CTxOut> getUnspentOutput(const COutPoint& outpoint) const = 0;

    //! Return whether node has the block and optionally return block metadata
    virtual bool findBlock(const uint256& hash, CBlock* block = nullptr, int* height = nullptr, int64_t* time = nullptr) const = 0;

    //! Notifications interface
    class Notifications {
    public:
        virtual ~Notifications() = default;
        virtual void transactionAddedToMempool(const CTransaction& tx) {}
        virtual void transactionRemovedFromMempool(const CTransaction& tx) {}
        virtual void blockConnected(const CBlock& block, int height) {}
        virtual void blockDisconnected(const CBlock& block, int height) {}
        virtual void updatedBlockTip() {}
        virtual void chainStateFlushed(const uint256& best_block_hash) {}
    };

    //! Register handler for notifications
    virtual std::unique_ptr<Handler> handleNotifications(std::shared_ptr<Notifications> notifications) = 0;

    //! Wait for notifications
    virtual void waitForNotificationsIfTipChanged(const uint256& old_tip) = 0;
};

//! Return implementation of Chain interface
std::unique_ptr<Chain> MakeChain();

} // namespace interfaces

#endif // ALIAS_INTERFACES_CHAIN_H
