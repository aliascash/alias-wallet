// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2009 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef BITCOINADDRESSVALIDATOR_H
#define BITCOINADDRESSVALIDATOR_H

#include <QRegExpValidator>

/** Base48 entry widget validator.
   Corrects near-miss characters and refuses characters that are no part of base48.
 */
class BitcoinAddressValidator : public QValidator
{
    Q_OBJECT
public:
    explicit BitcoinAddressValidator(QObject *parent = 0);

    State validate(QString &input, int &pos) const;

    //static const int MaxAddressLength = 35;
    static const int MaxAddressLength = 128; // accept stealth addresses
signals:

public slots:

};

#endif // BITCOINADDRESSVALIDATOR_H
