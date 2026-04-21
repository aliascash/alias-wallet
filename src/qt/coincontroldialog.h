// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2009 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef COINCONTROLDIALOG_H
#define COINCONTROLDIALOG_H

#include <QAbstractButton>
#include <QAction>
#include <QDialog>
#include <QList>
#include <QMenu>
#include <QPoint>
#include <QString>
#include <QTreeWidgetItem>

namespace Ui {
    class CoinControlDialog;
}

class WalletModel;
class SpectreBridge;
class CCoinControl;

class CoinControlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CoinControlDialog(QWidget *parent = nullptr);
    ~CoinControlDialog();

    void setModel(WalletModel *model);

    static void updateLabels(WalletModel*, QDialog*, SpectreBridge *bridge = nullptr);
    static QString getPriorityLabel(double);

    static QList<qint64> payAmounts;
    static CCoinControl *coinControl;

private:
    Ui::CoinControlDialog *ui;
    WalletModel *model{nullptr};
    int sortColumn{0};
    Qt::SortOrder sortOrder{Qt::AscendingOrder};

    QMenu *contextMenu{nullptr};
    QTreeWidgetItem *contextMenuItem{nullptr};
    QAction *copyTransactionHashAction{nullptr};

    QString strPad(QString, int, QString);
    void sortView(int, Qt::SortOrder);
    void updateView();

    enum
    {
        COLUMN_CHECKBOX,
        COLUMN_AMOUNT,
        COLUMN_LABEL,
        COLUMN_ADDRESS,
        COLUMN_DATE,
        COLUMN_CONFIRMATIONS,
        COLUMN_PRIORITY,
        COLUMN_TXHASH,
        COLUMN_VOUT_INDEX,
        COLUMN_AMOUNT_INT64,
        COLUMN_PRIORITY_INT64
    };

private Q_SLOTS:
    void showMenu(const QPoint &);
    void copyAmount();
    void copyLabel();
    void copyAddress();
    void copyTransactionHash();
    void clipboardQuantity();
    void clipboardAmount();
    void clipboardFee();
    void clipboardAfterFee();
    void clipboardBytes();
    void clipboardPriority();
    void clipboardLowOutput();
    void clipboardChange();
    void radioTreeMode(bool);
    void radioListMode(bool);
    void viewItemChanged(QTreeWidgetItem*, int);
    void headerSectionClicked(int);
    void buttonBoxClicked(QAbstractButton*);
    void buttonSelectAllClicked();
};

#endif // COINCONTROLDIALOG_H
