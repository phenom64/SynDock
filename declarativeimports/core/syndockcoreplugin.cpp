/*
    SPDX-FileCopyrightText: 2016 Smith AR <audoban@openmailbox.org>
    SPDX-FileCopyrightText: 2016 Michail Vourlakos <mvourlakos@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "syndockcoreplugin.h"

// local
#include "dialog.h"
#include "environment.h"
#include "iconitem.h"
#include "quickwindowsystem.h"
#include "tools.h"

#include <types.h>

// Qt
#include <QtQml>


void LatteCorePlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.syndromatic.syndock.core"));
    qmlRegisterUncreatableType<NSE::Types>(uri, 0, 2, "Types", "Latte Types uncreatable");
    qmlRegisterType<NSE::IconItem>(uri, 0, 2, "IconItem");
    qmlRegisterType<NSE::Quick::Dialog>(uri, 0, 2, "Dialog");
    qmlRegisterSingletonType<NSE::Environment>(uri, 0, 2, "Environment", &NSE::environment_qobject_singletontype_provider);
    qmlRegisterSingletonType<NSE::Tools>(uri, 0, 2, "Tools", &NSE::tools_qobject_singletontype_provider);
    qmlRegisterSingletonType<NSE::QuickWindowSystem>(uri, 0, 2, "WindowSystem", &NSE::windowsystem_qobject_singletontype_provider);
}
