/*
    SPDX-FileCopyrightText: 2020 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SYNDOCKTASKSPLUGIN_H
#define SYNDOCKTASKSPLUGIN_H

// Qt
#include <QQmlExtensionPlugin>

// Plasma
#include <PlasmaQuick/Dialog>

class SyndockTasksPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri) override;
};

#endif
