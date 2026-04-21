// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2011 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef QVALIDATEDLINEEDIT_H
#define QVALIDATEDLINEEDIT_H

#include <QLineEdit>

class QValidatedLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit QValidatedLineEdit(QWidget *parent = nullptr);
    void clear();

protected:
    void focusInEvent(QFocusEvent *evt) override;

private:
    bool valid{true};

public Q_SLOTS:
    void setValid(bool valid);

private Q_SLOTS:
    void markValid();
};

#endif // QVALIDATEDLINEEDIT_H
