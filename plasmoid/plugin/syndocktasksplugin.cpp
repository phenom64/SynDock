/*
    SPDX-FileCopyrightText: 2020 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "syndocktasksplugin.h"

// local
#include "types.h"

// Qt
#include <QtQml>


void SyndockTasksPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.kde.syndock.private.tasks"));
    qmlRegisterUncreatableType<Latte::Tasks::Types>(uri, 0, 1, "Types", "SynDock Tasks Types uncreatable");
}
