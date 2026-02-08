/*
    SPDX-FileCopyrightText: 2021 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick

import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore

import org.kde.syndock.core 0.2 as LatteCore
import org.kde.syndock.abilities.definition 0.1 as AbilityDefinition

AbilityDefinition.Containment {
    id: apis

    property int appletIndex: -1
    property Item myView: null

    alignment: {
        if (!myView) {
            return LatteCore.Types.Center;
        }

        if (myView.alignment === LatteCore.Types.Justify) {
            if (appletIndex>=0 && appletIndex<100) {
                return plasmoid.formFactor === PlasmaCore.Types.Horizontal ? LatteCore.Types.Left : LatteCore.Types.Top;
            } else if (appletIndex>=100 && appletIndex<200) {
                return LatteCore.Types.Center;
            } else if (appletIndex>=200) {
                return plasmoid.formFactor === PlasmaCore.Types.Horizontal ? LatteCore.Types.Right : LatteCore.Types.Bottom;
            }

            return LatteCore.Types.Center;
        }

        return myView.alignment;
    }

    readonly property Item publicApi: Item {
        readonly property alias isFirstAppletInContainment: apis.isFirstAppletInContainment
        readonly property alias isLastAppletInContainment: apis.isLastAppletInContainment

        readonly property alias alignment: apis.alignment
    }
}
