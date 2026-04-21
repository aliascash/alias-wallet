// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2011 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef OPTIONSMODEL_H
#define OPTIONSMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class OptionsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit OptionsModel(QObject *parent = nullptr);

    enum OptionID {
        Fee,
        ReserveBalance,
        StartAtStartup,
        DetachDatabases,
        Staking,
        StakingDonation,
        MinStakeInterval,
        ThinMode,
        ThinFullIndex,
        ThinIndexWindow,
        AutoRingSize,
        AutoRedeemSpectre,
        MinRingSize,
        MaxRingSize,
        MapPortUPnP,
        ProxyUse,
        ProxyIP,
        ProxyPort,
        ProxySocksVersion,
        MinimizeToTray,
        MinimizeOnClose,
        Language,
        DisplayUnit,
        DisplayAddresses,
        RowsPerPage,
        Notifications,
        VisibleTransactions,
        OptionIDRowCount,
    };

    QString optionIDName(int row);
    int optionNameID(QString name);

    void Init();

    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;

    qint64 getTransactionFee();
    qint64 getReserveBalance();
    bool getMinimizeToTray();
    bool getMinimizeOnClose();
    bool getDisplayAddresses();
    bool getAutoRingSize();
    bool getAutoRedeemSpectre();
    int getDisplayUnit();
    int getRowsPerPage();
    int getMinRingSize();
    int getMaxRingSize();
    int getStakingDonation();
    QStringList getNotifications();
    QStringList getVisibleTransactions();
    QString getLanguage() { return language; }

    void emitDisplayUnitChanged(int unit);
    void emitTransactionFeeChanged(qint64);
    void emitReserveBalanceChanged(qint64);
    void emitRowsPerPageChanged(int);
    void emitVisibleTransactionsChanged(QStringList);

private:
    int nDisplayUnit{0};
    int nRowsPerPage{20};
    int nMinRingSize{0};
    int nMaxRingSize{0};
    int fStakingDonation{0};
    bool bDisplayAddresses{false};
    bool fMinimizeToTray{false};
    bool fMinimizeOnClose{false};
    bool fAutoRingSize{false};
    bool fAutoRedeemSpectre{false};
    QString language;
    QStringList notifications;
    QStringList visibleTransactions;
    bool bActivateAllTransactiontypesAfterLanguageSwitch{false};
    bool bActivateAllNotificationsAfterLanguageSwitch{false};

Q_SIGNALS:
    void displayUnitChanged(int unit);
    void transactionFeeChanged(qint64);
    void reserveBalanceChanged(qint64);
    void rowsPerPageChanged(int);
    void visibleTransactionsChanged(QStringList);
};

#endif // OPTIONSMODEL_H
