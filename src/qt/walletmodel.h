// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2011 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef WALLETMODEL_H
#define WALLETMODEL_H

#include <QObject>

#include <vector>
#include <map>

#include "allocators.h"
#include "stealth.h"

class OptionsModel;
class AddressTableModel;
class TransactionTableModel;
class CWallet;
class CKeyID;
class CPubKey;
class COutput;
class COutPoint;
class uint256;
class CCoinControl;

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

enum eTxnTypeInd
{
    TXT_SPEC_TO_SPEC = 0,
    TXT_SPEC_TO_ANON,
    TXT_ANON_TO_ANON,
    TXT_ANON_TO_SPEC,
};

class SendCoinsRecipient
{
public:
    QString address;
    QString label;
    QString narration;
    int typeInd{0};
    qint64 amount{0};
    int txnTypeInd{0};
    int nRingSize{0};
};

class WalletModel : public QObject
{
    Q_OBJECT

public:
    explicit WalletModel(CWallet *wallet, OptionsModel *optionsModel, QObject *parent = nullptr);
    ~WalletModel();

    enum StatusCode
    {
        OK,
        InvalidAmount,
        InvalidAddress,
        StealthAddressOnlyAllowedForSPECTRE,
        RecipientAddressNotOwnedXSPECtoSPECTRE,
        RecipientAddressNotOwnedSPECTREtoXSPEC,
        AmountExceedsBalance,
        AmountWithFeeExceedsBalance,
        DuplicateAddress,
        TransactionCreationFailed,
        TransactionCommitFailed,
        NarrationTooLong,
        RingSizeError,
        InputTypeError,
        SCR_NeedFullMode,
        SCR_StealthAddressFail,
        SCR_StealthAddressFailAnonToSpec,
        SCR_AmountExceedsBalance,
        SCR_AmountWithFeeExceedsSpectreBalance,
        SCR_Error,
        SCR_ErrorWithMsg,
        Aborted
    };

    enum EncryptionStatus
    {
        Unencrypted,
        Locked,
        Unlocked
    };

    OptionsModel *getOptionsModel();
    AddressTableModel *getAddressTableModel();
    TransactionTableModel *getTransactionTableModel();

    qint64 getBalance() const;
    qint64 getSpectreBalance() const;
    qint64 getStake() const;
    qint64 getSpectreStake() const;
    qint64 getUnconfirmedBalance() const;
    qint64 getUnconfirmedSpectreBalance() const;
    qint64 getImmatureBalance() const;
    qint64 getImmatureSpectreBalance() const;
    int getNumTransactions() const;
    EncryptionStatus getEncryptionStatus() const;

    bool validateAddress(const QString &address);

    struct SendCoinsReturn
    {
        SendCoinsReturn(StatusCode status = Aborted, qint64 fee = 0, QString hex = QString())
            : status(status), fee(fee), hex(hex) {}
        StatusCode status;
        qint64 fee;
        QString hex;
    };

    SendCoinsReturn sendCoins(const QList<SendCoinsRecipient> &recipients, const CCoinControl *coinControl = nullptr);
    SendCoinsReturn sendCoinsAnon(const QList<SendCoinsRecipient> &recipients, const CCoinControl *coinControl = nullptr);

    bool setWalletEncrypted(bool encrypted, const SecureString &passphrase);
    bool setWalletLocked(bool locked, const SecureString &passPhrase = SecureString());
    bool changePassphrase(const SecureString &oldPass, const SecureString &newPass);
    bool backupWallet(const QString &filename);

    class UnlockContext
    {
    public:
        UnlockContext(WalletModel *wallet, bool valid, bool relock);
        ~UnlockContext();

        bool isValid() const { return valid; }

        UnlockContext(const UnlockContext& obj) { CopyFrom(obj); }
        UnlockContext& operator=(const UnlockContext& rhs) { CopyFrom(rhs); return *this; }

    private:
        WalletModel *wallet;
        bool valid;
        mutable bool relock;

        void CopyFrom(const UnlockContext& rhs);
    };

    enum UnlockMode { standard, rescan, login };
    UnlockContext requestUnlock(UnlockMode unlockMode = standard);
    int fUnlockRescanRequested{false};

    bool getPubKey(const CKeyID &address, CPubKey& vchPubKeyOut) const;
    void getOutputs(const std::vector<COutPoint>& vOutpoints, std::vector<COutput>& vOutputs);
    void listCoins(std::map<QString, std::vector<std::pair<COutput, bool>>>& mapCoins) const;
    bool getStealthAddress(const QString &address, CStealthAddress& stealthAddressOut) const;
    bool isLockedCoin(uint256 hash, unsigned int n) const;
    void lockCoin(COutPoint& output);
    void unlockCoin(COutPoint& output);
    void listLockedCoins(std::vector<COutPoint>& vOutpts);

    void emitBalanceChanged(qint64 balance, qint64 spectreBal, qint64 stake, qint64 spectreStake, qint64 unconfirmed, qint64 spectreUnconfirmed, qint64 immature, qint64 spectreImmature);
    void emitNumTransactionsChanged(int count);
    void emitEncryptionStatusChanged(int status);
    void emitRequireUnlock(UnlockMode mode);
    void emitError(const QString &title, const QString &message, bool modal);
    void checkBalanceChanged(bool force = false);

private:
    CWallet *wallet;
    OptionsModel *optionsModel;
    AddressTableModel *addressTableModel{nullptr};
    TransactionTableModel *transactionTableModel{nullptr};

    qint64 cachedBalance{0};
    qint64 cachedSpectreBal{0};
    qint64 cachedStake{0};
    qint64 cachedSpectreStake{0};
    qint64 cachedUnconfirmedBalance{0};
    qint64 cachedUnconfirmedSpectreBalance{0};
    qint64 cachedImmatureBalance{0};
    qint64 cachedImmatureSpectreBalance{0};
    qint64 cachedNumTransactions{0};
    EncryptionStatus cachedEncryptionStatus{Unencrypted};
    int cachedNumBlocks{0};
    bool fForceCheckBalanceChanged{false};

    void subscribeToCoreSignals();
    void unsubscribeFromCoreSignals();

public Q_SLOTS:
    void updateStatus();
    void updateTransaction(const QString &hash, int status);
    void updateAddressBook(const QString &address, const QString &label, bool isMine, int status, bool fManual);
    void pollBalanceChanged();
    void requestUnlockRescan();

Q_SIGNALS:
    void balanceChanged(qint64 balance, qint64 spectreBal, qint64 stake, qint64 spectreStake, qint64 unconfirmed, qint64 spectreUnconfirmed, qint64 immature, qint64 spectreImmature);
    void numTransactionsChanged(int count);
    void encryptionStatusChanged(int status);
    void requireUnlock(WalletModel::UnlockMode mode);
    void error(const QString &title, const QString &message, bool modal);
};

#endif // WALLETMODEL_H
