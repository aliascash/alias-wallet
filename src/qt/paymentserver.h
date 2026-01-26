// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2011 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef PAYMENTSERVER_H
#define PAYMENTSERVER_H

#include <QObject>
#include <QString>

class OptionsModel;
class QApplication;
class QLocalServer;

class PaymentServer : public QObject
{
    Q_OBJECT

public:
    static bool ipcSendCommandLine();

    PaymentServer(QApplication* parent);

    bool eventFilter(QObject *object, QEvent *event) override;
    void setOptionsModel(OptionsModel *optionsModel);

Q_SIGNALS:
    void receivedURI(QString);

public Q_SLOTS:
    void uiReady();

private Q_SLOTS:
    void handleURIConnection();

private:
    bool saveURIs{true};
    QLocalServer* uriServer{nullptr};
    OptionsModel *optionsModel{nullptr};
};

#endif // PAYMENTSERVER_H
