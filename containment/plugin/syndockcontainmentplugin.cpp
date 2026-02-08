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

void SyndockContainmentPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.kde.syndock.private.containment"));
    qmlRegisterUncreatableType<Latte::Containment::Types>(uri, 0, 1, "Types", "SynDock Containment Types uncreatable");
    qmlRegisterType<Latte::Containment::LayoutManager>(uri, 0, 1, "LayoutManager");
}
