// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2009 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
    class AboutDialog;
}

class ClientModel;

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

    void setModel(ClientModel *model);

private:
    Ui::AboutDialog *ui;

private Q_SLOTS:
    void on_buttonBox_accepted();
};

#endif // ABOUTDIALOG_H
