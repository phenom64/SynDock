/*
    SPDX-FileCopyrightText: 2020 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "syndockcontainmentplugin.h"

// local
#include "layoutmanager.h"
#include "types.h"

// Qt
#include <QtQml>

void LatteContainmentPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.syndromatic.syndock.private.containment"));
    qmlRegisterUncreatableType<NSE::Containment::Types>(uri, 0, 1, "Types", "Latte Containment Types uncreatable");
    qmlRegisterType<NSE::Containment::LayoutManager>(uri, 0, 1, "LayoutManager");
}

