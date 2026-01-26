// SPDX-FileCopyrightText: © 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, <info@kdab.com>
//
// SPDX-License-Identifier: BSD-4-Clause

#ifndef WEBSOCKETCLIENTWRAPPER_H
#define WEBSOCKETCLIENTWRAPPER_H

#include <QObject>

class WebSocketTransport;

QT_BEGIN_NAMESPACE
class QWebSocketServer;
QT_END_NAMESPACE

class WebSocketClientWrapper : public QObject
{
    Q_OBJECT

public:
    WebSocketClientWrapper(QWebSocketServer *server, QString accessToken, QObject *parent = nullptr);

Q_SIGNALS:
    void clientConnected(WebSocketTransport *client);

private Q_SLOTS:
    void handleNewConnection();

private:
    QWebSocketServer *m_server{nullptr};
    QString accessToken;
};

#endif // WEBSOCKETCLIENTWRAPPER_H
