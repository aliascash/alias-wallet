// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2011 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef TRANSACTIONTABLEMODEL_H
#define TRANSACTIONTABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

#include <vector>

class CWallet;
class TransactionTablePriv;
class TransactionRecord;
class WalletModel;

struct TransactionNotification
{
public:
    TransactionNotification() {}
    TransactionNotification(QString hash, int status, bool showTransaction)
        : hash(hash), status(status), showTransaction(showTransaction) {}

    QString hash;
    int status{0};
    bool showTransaction{false};
};

class TransactionTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TransactionTableModel(CWallet* wallet, WalletModel *parent = nullptr);
    ~TransactionTableModel();

    enum ColumnIndex {
        Status = 0,
        Date = 1,
        Amount = 2,
        Type = 3,
        ToAddress = 4,
        Narration = 5
    };

    enum RoleIndex {
        TypeRole = Qt::UserRole,
        DateRole,
        LongDescriptionRole,
        AddressRole,
        LabelRole,
        AmountRole,
        CurrencyRole,
        UnitRole,
        TxIDRole,
        ConfirmedRole,
        FormattedAmountRole,
        StatusRole,
        ConfirmationsRole
    };

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const override;

    int lookupTransaction(const QString &txid) const;

private:
    CWallet* wallet;
    WalletModel *walletModel;
    QStringList columns;
    TransactionTablePriv *priv{nullptr};
    std::vector<TransactionNotification> vQueueNotifications;
    bool fProcessTransactionNotifications{false};

    QString lookupAddress(const std::string &address, bool tooltip) const;
    QVariant addressColor(const TransactionRecord *wtx) const;
    QString formatTxStatus(const TransactionRecord *wtx) const;
    QString formatTxDate(const TransactionRecord *wtx) const;
    QString formatTxToAddress(const TransactionRecord *wtx, bool tooltip) const;
    QString formatTxAmount(const TransactionRecord *wtx, bool showUnconfirmed = true) const;
    QString formatNarration(const TransactionRecord *wtx) const;
    QString formatTooltip(const TransactionRecord *rec) const;
    QString txStatusDecoration(const TransactionRecord *wtx) const;
    QVariant txAddressDecoration(const TransactionRecord *wtx) const;

    void subscribeToCoreSignals();
    void unsubscribeFromCoreSignals();
    void emitDataChanged(int rowTop, int rowBottom);

public Q_SLOTS:
    void updateTransaction(const QString &hash, int status, bool showTransaction);
    void updateConfirmations();
    void updateDisplayUnit();

    friend class TransactionTablePriv;
};

#endif
