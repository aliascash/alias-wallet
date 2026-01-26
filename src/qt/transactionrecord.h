// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2011 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef TRANSACTIONRECORD_H
#define TRANSACTIONRECORD_H

#include "wallet.h"
#include "uint256.h"

#include <QList>
#include <QString>

class CWallet;
class CWalletTx;

class TransactionStatus
{
public:
    TransactionStatus()
        : countsForBalance(false), sortKey(""), matures_in(0),
          status(Offline), depth(0), open_for(0), cur_num_blocks(-1) {}

    enum Status {
        Confirmed,
        OpenUntilDate,
        OpenUntilBlock,
        Offline,
        Unconfirmed,
        Confirming,
        Conflicted,
        Immature,
        MaturesWarning,
        NotAccepted
    };

    bool countsForBalance;
    std::string sortKey;
    int matures_in;
    Status status;
    int64_t depth;
    int64_t open_for;
    int cur_num_blocks;
};

class TransactionRecord
{
public:
    enum Type
    {
        Other,
        Generated,
        GeneratedSPECTRE,
        GeneratedDonation,
        GeneratedSPECTREDonation,
        GeneratedContribution,
        GeneratedSPECTREContribution,
        SendToAddress,
        SendToOther,
        RecvWithAddress,
        RecvFromOther,
        SendToSelf,
        SendToSelfSPECTRE,
        RecvSpectre,
        SendSpectre,
        ConvertXSPECtoSPECTRE,
        ConvertSPECTREtoXSPEC
    };

    static QString getTypeLabel(const int &type);
    static QString getTypeShort(const int &type);

    static const int RecommendedNumConfirmations = 6;

    TransactionRecord()
        : hash(), time(0), type(Other), address(""), narration(""),
          debit(0), credit(0), currency(PUBLIC), idx(0) {}

    TransactionRecord(uint256 hash, int64_t time)
        : hash(hash), time(time), type(Other), address(""), narration(""),
          debit(0), credit(0), currency(PUBLIC), idx(0) {}

    TransactionRecord(uint256 hash, int64_t time, Type type, const std::string &address,
                      const std::string &narration, int64_t debit, int64_t credit,
                      const Currency &currency)
        : hash(hash), time(time), type(type), address(address), narration(narration),
          debit(debit), credit(credit), currency(currency), idx(0) {}

    TransactionRecord(uint256 hash, int64_t time, Type type, const std::string &address,
                      const std::string &narration, int64_t debit, int64_t credit,
                      const Currency &currency, int idx)
        : hash(hash), time(time), type(type), address(address), narration(narration),
          debit(debit), credit(credit), currency(currency), idx(idx) {}

    static bool showTransaction(const CWalletTx &wtx);
    static QList<TransactionRecord> decomposeTransaction(const CWallet *wallet, const CWalletTx &wtx);

    uint256 hash;
    qint64 time;
    Type type;
    std::string address;
    std::string narration;
    qint64 debit;
    qint64 credit;
    Currency currency;
    int idx;
    TransactionStatus status;

    std::string getTxID();
    QString getTypeLabel();
    void updateStatus(const CWalletTx &wtx);
    bool statusUpdateNeeded();
};

#endif // TRANSACTIONRECORD_H
