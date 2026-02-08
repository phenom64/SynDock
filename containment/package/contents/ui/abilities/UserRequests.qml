/*
    SPDX-FileCopyrightText: 2020 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick

import org.kde.syndock.core 0.2 as LatteCore
import org.kde.syndock.abilities.definition 0.1 as AbilityDefinition

AbilityDefinition.UserRequests {
    id: containerUserRequests
    property QtObject view: null

    Connections {
        target: view
        onUserRequestedViewType: {
            containerUserRequests.sglViewType(type);
        }
    }
}
