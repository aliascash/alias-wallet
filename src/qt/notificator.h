// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2011 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef NOTIFICATOR_H
#define NOTIFICATOR_H

#include <QIcon>
#include <QObject>

QT_BEGIN_NAMESPACE
class QSystemTrayIcon;

#ifdef USE_DBUS
class QDBusInterface;
#endif
QT_END_NAMESPACE

class Notificator: public QObject
{
    Q_OBJECT

public:
    Notificator(const QString &programName, QSystemTrayIcon *trayIcon, QWidget *parent);
    ~Notificator();

    enum Class
    {
        Information,
        Warning,
        Critical
    };

public Q_SLOTS:
    void notify(Class cls, const QString &title, const QString &text,
                const QIcon &icon = QIcon(), int millisTimeout = 10000);

private:
    QWidget *parent;
    enum Mode {
        None,
        Freedesktop,
        QSystemTray,
        UserNotificationCenter
    };
    QString programName;
    Mode mode{None};
    QSystemTrayIcon *trayIcon{nullptr};
#ifdef USE_DBUS
    QDBusInterface *interface{nullptr};

    void notifyDBus(Class cls, const QString &title, const QString &text, const QIcon &icon, int millisTimeout);
#endif
    void notifySystray(Class cls, const QString &title, const QString &text, const QIcon &icon, int millisTimeout);
#ifdef Q_OS_MACOS
    void notifyMacUserNotificationCenter(Class cls, const QString &title, const QString &text, const QIcon &icon);
#endif
};

#endif // NOTIFICATOR_H
