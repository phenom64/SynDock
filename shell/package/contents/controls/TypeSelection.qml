/*
    SPDX-FileCopyrightText: 2016 Smith AR <audoban@openmailbox.org>
    SPDX-FileCopyrightText: 2016 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.components as PlasmaComponents

import org.kde.plasma.plasmoid

import org.kde.syndock.core 0.2 as LatteCore
import org.kde.syndock.private.containment 0.1 as LatteContainment

Grid {
    id: typeRow

    width: horizontal ? content.width - 4*units.smallSpacing : 9 * theme.defaultFont.pixelSize
    anchors.leftMargin: horizontal ? units.smallSpacing : 0
    anchors.rightMargin: horizontal ? units.smallSpacing : 0

    Layout.minimumWidth: width
    Layout.maximumWidth: width
    Layout.minimumHeight: height
    Layout.maximumHeight: height

    Layout.leftMargin: units.smallSpacing * 2
    Layout.rightMargin: units.smallSpacing * 2

    rows: horizontal ? 1 : 0
    columns: horizontal ? 0 : 1

    spacing: 1

    verticalItemAlignment: Grid.AlignVCenter

    property bool horizontal: false

    ExclusiveGroup {
        id: viewTypeGroup
    }

    PlasmaComponents.Button {
        id: dockTypeButton
        width: horizontal ? (parent.width - parent.spacing)/ 2 : parent.width
        enabled: LatteCore.WindowSystem.compositingActive

        checkable: true
        checked: dockView.type === LatteCore.Types.DockView
        text: i18nc("dock type","Dock")
        exclusiveGroup: viewTypeGroup
        tooltip: i18n("Change the behavior and appearance to Dock type")

        onPressedChanged: {
            if (pressed && !checked) {
                dockView.userRequestedViewType(LatteCore.Types.DockView);

                dockView.visibility.mode = LatteCore.Types.DodgeActive;
                plasmoid.configuration.alignment = LatteCore.Types.Center;
                plasmoid.configuration.useThemePanel = true;
                plasmoid.configuration.solidPanel = false;
                plasmoid.configuration.panelSize = 5;
                plasmoid.configuration.appletShadowsEnabled = true;
                plasmoid.configuration.zoomLevel = 16;
                //plasmoid.configuration.autoDecreaseIconSize = true;

                //! Empty Areas
                plasmoid.configuration.dragActiveWindowEnabled = false;
                plasmoid.configuration.scrollAction = LatteContainment.Types.ScrollNone;

                //! Items
                plasmoid.configuration.autoSizeEnabled = true;

                //! Dynamic Background
                plasmoid.configuration.solidBackgroundForMaximized = false;
                plasmoid.configuration.colorizeTransparentPanels = false;
                plasmoid.configuration.backgroundOnlyOnMaximized = false;
                plasmoid.configuration.disablePanelShadowForMaximized = false;
                plasmoid.configuration.plasmaBackgroundForPopups = false;

                //! Floating
                plasmoid.configuration.floatingInternalGapIsForced = true;
            }
        }
    }

    PlasmaComponents.Button {
        id: panelTypeButton
        width: dockTypeButton.width
        enabled: LatteCore.WindowSystem.compositingActive

        checkable: true
        checked: dockView.type === LatteCore.Types.PanelView
        text: i18nc("panel type","Panel")
        exclusiveGroup: viewTypeGroup
        tooltip: i18n("Change the behavior and appearance to Panel type")

        onPressedChanged: {
            if (pressed && !checked) {
                dockView.userRequestedViewType(LatteCore.Types.PanelView);

                dockView.visibility.mode = LatteCore.Types.AlwaysVisible;
                plasmoid.configuration.alignment = LatteCore.Types.Justify;
                plasmoid.configuration.useThemePanel = true;
                plasmoid.configuration.solidPanel = false;
                plasmoid.configuration.panelSize = 100;
                plasmoid.configuration.panelShadows = true;
                plasmoid.configuration.appletShadowsEnabled = false;
                plasmoid.configuration.zoomLevel = 0;
                plasmoid.configuration.titleTooltips = false;
                //plasmoid.configuration.autoDecreaseIconSize = false;

                //! Empty Areas
                plasmoid.configuration.dragActiveWindowEnabled = true;

                //! Items
                plasmoid.configuration.autoSizeEnabled = false;

                //! Dynamic Background
                plasmoid.configuration.colorizeTransparentPanels = false;
                plasmoid.configuration.backgroundOnlyOnMaximized = false;
                plasmoid.configuration.disablePanelShadowForMaximized = false;
                plasmoid.configuration.plasmaBackgroundForPopups = true;

                //! Floating
                plasmoid.configuration.floatingInternalGapIsForced = false;

                //! Custom Background that overrides Plasma Theme metrics
                plasmoid.configuration.backgroundRadius = -1;
                plasmoid.configuration.backgroundShadowSize = -1;
            }
        }
    }
}

