// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2009 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef BITCOINFIELD_H
#define BITCOINFIELD_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QDoubleSpinBox;
class QValueComboBox;
QT_END_NAMESPACE

class BitcoinAmountField: public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qint64 value READ value WRITE setValue NOTIFY textChanged USER true)

public:
    explicit BitcoinAmountField(QWidget *parent = nullptr);

    qint64 value(bool *valid = nullptr) const;
    void setValue(qint64 value);
    void setValid(bool valid);
    bool validate();
    void setDisplayUnit(int unit);
    void clear();
    QWidget *setupTabChain(QWidget *prev);

Q_SIGNALS:
    void textChanged();

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    QDoubleSpinBox *amount{nullptr};
    QValueComboBox *unit{nullptr};
    int currentUnit{0};

    void setText(const QString &text);
    QString text() const;

private Q_SLOTS:
    void unitChanged(int idx);
};

#endif // BITCOINFIELD_H
