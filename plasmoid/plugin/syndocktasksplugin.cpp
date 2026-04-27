/*
    SPDX-FileCopyrightText: 2020 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "syndocktasksplugin.h"

// local
#include "types.h"

// Qt
#include <QtQml>


void LatteTasksPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.syndromatic.syndock.private.tasks"));
    qmlRegisterUncreatableType<NSE::Tasks::Types>(uri, 0, 1, "Types", "Latte Tasks Types uncreatable");
}

