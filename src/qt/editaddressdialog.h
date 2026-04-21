// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2009 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef EDITADDRESSDIALOG_H
#define EDITADDRESSDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QDataWidgetMapper;
QT_END_NAMESPACE

namespace Ui {
    class EditAddressDialog;
}

class AddressTableModel;

class EditAddressDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode {
        NewReceivingAddress,
        NewSendingAddress,
        EditReceivingAddress,
        EditSendingAddress
    };

    explicit EditAddressDialog(Mode mode, QWidget *parent = nullptr);
    ~EditAddressDialog();

    void setModel(AddressTableModel *model);
    void loadRow(int row);

    QString getAddress() const;
    void setAddress(const QString &address);

public Q_SLOTS:
    void accept() override;

private:
    bool saveCurrentRow();

    Ui::EditAddressDialog *ui;
    QDataWidgetMapper *mapper{nullptr};
    Mode mode;
    AddressTableModel *model{nullptr};
    QString address;
};

#endif // EDITADDRESSDIALOG_H
