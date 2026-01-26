// SPDX-FileCopyrightText: © 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, <info@kdab.com>
//
// SPDX-License-Identifier: BSD-4-Clause

#ifndef WEBSOCKETTRANSPORT_H
#define WEBSOCKETTRANSPORT_H

#include <QWebChannelAbstractTransport>

QT_BEGIN_NAMESPACE
class QWebSocket;
QT_END_NAMESPACE

class WebSocketTransport : public QWebChannelAbstractTransport
{
    Q_OBJECT

public:
    explicit WebSocketTransport(QWebSocket *socket);
    virtual ~WebSocketTransport();

    void sendMessage(const QJsonObject &message) override;

private Q_SLOTS:
    void textMessageReceived(const QString &message);

private:
    QWebSocket *m_socket{nullptr};
};

#endif // WEBSOCKETTRANSPORT_H
