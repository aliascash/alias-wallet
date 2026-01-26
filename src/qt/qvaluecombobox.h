// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2011 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef QVALUECOMBOBOX_H
#define QVALUECOMBOBOX_H

#include <QComboBox>
#include <QVariant>

class QValueComboBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged USER true)

public:
    explicit QValueComboBox(QWidget *parent = nullptr);

    QVariant value() const;
    void setValue(const QVariant &value);
    void setRole(int role);

Q_SIGNALS:
    void valueChanged();

private:
    int role{Qt::UserRole};

private Q_SLOTS:
    void handleSelectionChanged(int idx);
};

#endif // QVALUECOMBOBOX_H
