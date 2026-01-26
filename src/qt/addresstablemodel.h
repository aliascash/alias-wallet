// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2009 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef ADDRESSTABLEMODEL_H
#define ADDRESSTABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class AddressTablePriv;
class CWallet;
class WalletModel;

enum EAddressType {
    AT_Unknown = 0,
    AT_Normal = 1,
    AT_Stealth = 2,
    AT_BIP32 = 3,
};

class AddressTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit AddressTableModel(CWallet *wallet, WalletModel *parent = nullptr);
    ~AddressTableModel();

    enum ColumnIndex {
        Label = 0,
        Address = 1,
        Pubkey = 2,
        AddressType = 3,
        Type = 4
    };

    enum RoleIndex {
        TypeRole = Qt::UserRole,
        AddressTypeRole,
    };

    enum EditStatus {
        OK,
        NO_CHANGES,
        INVALID_ADDRESS,
        DUPLICATE_ADDRESS,
        WALLET_UNLOCK_FAILURE,
        KEY_GENERATION_FAILURE
    };

    static const QString Send;
    static const QString Receive;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QString addRow(const QString &type, const QString &label, const QString &address, int addressType);
    QString labelForAddress(const QString &address) const;
    QString pubkeyForAddress(const QString &address, const bool lookup = true) const;
    int lookupAddress(const QString &address) const;

    EditStatus getEditStatus() const { return editStatus; }

private:
    WalletModel *walletModel;
    CWallet *wallet;
    AddressTablePriv *priv{nullptr};
    QStringList columns;
    EditStatus editStatus{OK};

    void emitDataChanged(int row);

Q_SIGNALS:
    void defaultAddressChanged(const QString &address);

public Q_SLOTS:
    void updateEntry(const QString &address, const QString &label, bool isMine, int status);
    void setEncryptionStatus(int status);

    friend class AddressTablePriv;
};

#endif // ADDRESSTABLEMODEL_H
