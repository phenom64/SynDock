/*
    SPDX-FileCopyrightText: 2019 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.components as PlasmaComponents

PlasmaComponents.Label {
    Layout.fillWidth: true
    Layout.topMargin: isFirstSubCategory ? 0 : units.smallSpacing * 2
    Layout.bottomMargin: units.smallSpacing
    horizontalAlignment: Text.AlignHCenter
    opacity: 0.4

    property bool isFirstSubCategory: false
}
