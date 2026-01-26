// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// Copyright (c) 2018-2022 The Bitcoin Core developers
// SPDX-License-Identifier: MIT

#ifndef ALIAS_INTERFACES_WALLET_H
#define ALIAS_INTERFACES_WALLET_H

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <cstdint>

class CKey;
class CKeyID;
class CPubKey;
class CScript;
class CTransaction;
class COutPoint;
class CTxDestination;
class uint256;

namespace interfaces {

class Handler;

//! Collection of wallet info for display
struct WalletInfo {
    std::string name;
    bool have_watch_only{false};
    bool is_encrypted{false};
    bool is_locked{true};
};

//! Collection of wallet balance info
struct WalletBalance {
    int64_t balance{0};
    int64_t unconfirmed_balance{0};
    int64_t immature_balance{0};
    int64_t stake{0};
    int64_t anon_balance{0};
    int64_t anon_unconfirmed_balance{0};
    bool have_watch_only{false};
    int64_t watch_only_balance{0};
    int64_t watch_only_unconfirmed_balance{0};
    int64_t watch_only_immature_balance{0};
};

//! Wallet transaction output
struct WalletTxOut {
    CScript script_pub_key;
    int64_t value{0};
    int depth{0};
    bool is_spent{false};
    bool is_anon{false};
};

//! Wallet transaction
struct WalletTx {
    uint256 hash;
    int64_t time{0};
    int64_t value{0};
    int64_t fee{0};
    int depth{0};
    bool is_coinbase{false};
    bool is_coinstake{false};
    bool is_anon{false};
    bool is_in_mempool{false};
    std::map<std::string, std::string> additional_info;
};

//! Interface for accessing a wallet
class Wallet {
public:
    virtual ~Wallet() = default;

    //! Encrypt wallet
    virtual bool encryptWallet(const std::string& passphrase) = 0;

    //! Check if wallet is encrypted
    virtual bool isEncrypted() const = 0;

    //! Lock wallet
    virtual bool lock() = 0;

    //! Unlock wallet
    virtual bool unlock(const std::string& passphrase, bool staking_only = false) = 0;

    //! Check if wallet is locked
    virtual bool isLocked() const = 0;

    //! Change wallet passphrase
    virtual bool changeWalletPassphrase(const std::string& old_passphrase, const std::string& new_passphrase) = 0;

    //! Get wallet name
    virtual std::string getWalletName() const = 0;

    //! Get wallet balance info
    virtual WalletBalance getBalance() const = 0;

    //! Get available balance
    virtual int64_t getAvailableBalance() const = 0;

    //! Check if we have key
    virtual bool haveKey(const CKeyID& key_id) const = 0;

    //! Get key
    virtual bool getKey(const CKeyID& key_id, CKey& key) const = 0;

    //! Get pubkey
    virtual bool getPubKey(const CKeyID& key_id, CPubKey& pub_key) const = 0;

    //! Get address for destination
    virtual std::string getAddress(const CTxDestination& dest) const = 0;

    //! Get address book name
    virtual std::string getAddressLabel(const CTxDestination& dest) const = 0;

    //! Get transaction
    virtual bool getTransaction(const uint256& txid, WalletTx& wtx) const = 0;

    //! Get wallet transactions
    virtual std::vector<WalletTx> getWalletTxs() const = 0;

    //! Check if transaction is from this wallet
    virtual bool isFromMe(const CTransaction& tx) const = 0;

    //! Check if output is mine
    virtual bool isMine(const CScript& script) const = 0;

    //! Create transaction
    virtual bool createTransaction(
        const std::vector<std::pair<CScript, int64_t>>& recipients,
        int64_t& fee,
        std::string& fail_reason,
        CTransaction& tx) = 0;

    //! Commit transaction
    virtual bool commitTransaction(const CTransaction& tx, std::string& reject_reason) = 0;

    //! Notifications interface
    class Notifications {
    public:
        virtual ~Notifications() = default;
        virtual void walletLockStatusChanged(bool locked) {}
        virtual void walletEncryptionStatusChanged() {}
        virtual void balanceChanged() {}
        virtual void transactionChanged() {}
        virtual void addressBookChanged() {}
        virtual void showProgress(const std::string& title, int progress) {}
    };

    //! Register handler for notifications
    virtual std::unique_ptr<Handler> handleNotifications(std::shared_ptr<Notifications> notifications) = 0;
};

//! Wallet loader interface
class WalletLoader {
public:
    virtual ~WalletLoader() = default;

    //! Get wallet names
    virtual std::vector<std::string> listWallets() const = 0;

    //! Load wallet
    virtual std::unique_ptr<Wallet> loadWallet(const std::string& name, std::string& error, std::string& warning) = 0;

    //! Create wallet
    virtual std::unique_ptr<Wallet> createWallet(const std::string& name, std::string& error, std::string& warning) = 0;

    //! Get wallet
    virtual std::unique_ptr<Wallet> getWallet(const std::string& name) const = 0;
};

//! Return implementation of Wallet interface
std::unique_ptr<Wallet> MakeWallet(const std::string& name);

//! Return implementation of WalletLoader interface  
std::unique_ptr<WalletLoader> MakeWalletLoader();

} // namespace interfaces

#endif // ALIAS_INTERFACES_WALLET_H
