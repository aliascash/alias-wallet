// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2011 Bitcoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef MONITOREDDATAMAPPER_H
#define MONITOREDDATAMAPPER_H

#include <QDataWidgetMapper>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

class MonitoredDataMapper : public QDataWidgetMapper
{
    Q_OBJECT

public:
    explicit MonitoredDataMapper(QObject *parent = nullptr);

    void addMapping(QWidget *widget, int section);
    void addMapping(QWidget *widget, int section, const QByteArray &propertyName);

private:
    void addChangeMonitor(QWidget *widget);

Q_SIGNALS:
    void viewModified();
};

#endif // MONITOREDDATAMAPPER_H
