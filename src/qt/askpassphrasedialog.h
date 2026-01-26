// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2009 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef ASKPASSPHRASEDIALOG_H
#define ASKPASSPHRASEDIALOG_H

#include <QDialog>

namespace Ui {
    class AskPassphraseDialog;
}

class WalletModel;

class AskPassphraseDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode {
        Encrypt,
        Unlock,
        UnlockStaking,
        UnlockRescan,
        UnlockLogin,
        ChangePass,
        Decrypt
    };

    explicit AskPassphraseDialog(Mode mode, QWidget *parent = nullptr);
    ~AskPassphraseDialog();

    void accept() override;
    void setModel(WalletModel *model);

private:
    Ui::AskPassphraseDialog *ui;
    Mode mode;
    WalletModel *model{nullptr};
    bool fCapsLock{false};

private Q_SLOTS:
    void textChanged();
    bool event(QEvent *event) override;
    bool eventFilter(QObject *, QEvent *event) override;
    void secureClearPassFields();
};

#endif // ASKPASSPHRASEDIALOG_H
