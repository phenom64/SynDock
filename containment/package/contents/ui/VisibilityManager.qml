/*
 * This file is a part of the Atmo desktop experience's SynDock project for SynOS.
 * Copyright (C) 2026 Syndromatic Ltd. All rights reserved
 * Designed by Kavish Krishnakumar in Manchester.
 *
 * Based on Latte Dock:
 * SPDX-FileCopyrightText: 2016 Smith AR <audoban@openmailbox.org>
 * SPDX-FileCopyrightText: 2016 Michail Vourlakos <mvourlakos@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Window

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid

import org.kde.syndock.core 0.2 as LatteCore
import org.kde.syndock.private.containment 0.1 as LatteContainment

Item{
    id: manager
    anchors.fill: parent

    property QtObject window

    property bool isFloatingInClientSide: !root.behaveAsPlasmaPanel
                                          && screenEdgeMarginEnabled
                                          && !root.floatingInternalGapIsForced
                                          && !inSlidingIn
                                          && !inSlidingOut

    property int animationSpeed: LatteCore.WindowSystem.compositingActive ?
                                     (root.editMode ? 400 : animations.speedFactor.current * 1.62 * animations.duration.large) : 0

    property bool inClientSideScreenEdgeSliding: root.behaveAsDockWithMask && hideThickScreenGap
    property bool inNormalState: ((animations.needBothAxis.count === 0) && (animations.needLength.count === 0))
                                 || (dockView && dockView.visibility.isHidden && !dockView.visibility.containsMouse && animations.needThickness.count === 0)
    property bool inRelocationAnimation: dockView && dockView.positioner && dockView.positioner.inRelocationAnimation

    property bool inSlidingIn: false //necessary because of its init structure
    property alias inSlidingOut: slidingAnimationAutoHiddenOut.running
    property bool inRelocationHiding: false

    readonly property bool isSinkedEventEnabled: !(parabolic.isEnabled && (animations.needBothAxis.count>0 || animations.needLength.count>0))
                                                 && myView.isShownFully

    property int length: root.isVertical ?  Screen.height : Screen.width   //screenGeometry.height : screenGeometry.width

    property int slidingOutToPos: {
        if (root.behaveAsPlasmaPanel) {
            var edgeMargin = screenEdgeMarginEnabled ? plasmoid.configuration.screenEdgeMargin : 0
            var thickmarg = dockView.visibility.isSidebar ? 0 : 1;

            return root.isHorizontal ? root.height + edgeMargin - thickmarg : root.width + edgeMargin - thickmarg;
        } else {
            var topOrLeftEdge = ((plasmoid.location===PlasmaCore.Types.LeftEdge)||(plasmoid.location===PlasmaCore.Types.TopEdge));
            return (topOrLeftEdge ? -metrics.mask.thickness.normal : metrics.mask.thickness.normal);
        }
    }

    //! when Latte behaves as Plasma panel
    property int thicknessAsPanel: metrics.totals.thickness

    property Item layouts: null

    property bool updateIsEnabled: autosize.inCalculatedIconSize && !inSlidingIn && !inSlidingOut && !inRelocationHiding

    Connections{
        target: background.totals
        onVisualLengthChanged: updateMaskArea();
        onVisualThicknessChanged: updateMaskArea();
    }

    Connections{
        target: background.shadows
        onHeadThicknessChanged: updateMaskArea();
    }

    Connections{
        target: dockView ? dockView : null
        onXChanged: updateMaskArea();
        onYChanged: updateMaskArea()
        onWidthChanged: updateMaskArea();
        onHeightChanged: updateMaskArea();
    }

    Connections{
        target: animations.needBothAxis
        onCountChanged: updateMaskArea();
    }

    Connections{
        target: animations.needLength
        onCountChanged: updateMaskArea();
    }

    Connections{
        target: animations.needThickness
        onCountChanged: updateMaskArea();
    }

    Connections{
        target: layoutsManager
        onCurrentLayoutIsSwitching: {
            if (LatteCore.WindowSystem.compositingActive && dockView && dockView.layout && dockView.layout.name === layoutName) {
                parabolic.sglClearZoom();
            }
        }
    }

    Connections {
        target: metrics.mask.thickness
        onMaxZoomedChanged: updateMaskArea()
    }

    Connections {
        target: root.myView
        onInRelocationAnimationChanged: {
            if (!root.myView.inRelocationAnimation) {
                manager.updateMaskArea();
            }
        }
    }

    Connections {
        target: dockView ? dockView.effects : null
        onRectChanged: manager.updateMaskArea()
    }

    Connections{
        target: themeExtended ? themeExtended : null
        onThemeChanged: dockView.effects.forceMaskRedraw();
    }

    Connections {
        target: LatteCore.WindowSystem
        onCompositingActiveChanged: {
            manager.updateMaskArea();
        }
    }

    onIsFloatingInClientSideChanged: updateMaskArea();

    onInNormalStateChanged: {
        if (inNormalState) {
            updateMaskArea();
        }
    }

    onInSlidingInChanged: {
        if (dockView && !inSlidingIn && dockView.positioner.inRelocationShowing) {
            dockView.positioner.inRelocationShowing = false;
        }
    }

    onUpdateIsEnabledChanged: {
        if (updateIsEnabled) {
            updateMaskArea();
        }
    }

    function slotContainsMouseChanged() {
        if(dockView.visibility.containsMouse && dockView.visibility.mode !== LatteCore.Types.SidebarOnDemand) {
            updateMaskArea();

            if (slidingAnimationAutoHiddenOut.running && !inRelocationHiding) {
                slotMustBeShown();
            }
        }
    }

    function slotMustBeShown() {
        if (root.inStartup) {
            slidingAnimationAutoHiddenIn.init();
            return;
        }

        //! WindowsCanCover case
        if (dockView && dockView.visibility.mode === LatteCore.Types.WindowsCanCover) {
            dockView.visibility.setViewOnFrontLayer();
        }

        if (!dockView.visibility.isHidden && dockView.positioner.inSlideAnimation) {
            // Do not update when Positioner mid-slide animation takes place, for example:
            // 1. Latte panel is hiding its floating gap for maximized window
            // 2. the user clicks on an applet popup.
            // 3. Applet popups showing/hiding are triggering hidingIsBlockedChanged() signals.
            // 4. hidingIsBlockedChanged() signals create mustBeShown events when visibility::hidingIsBlocked() is not enabled.
            return;
        }

        //! Normal Dodge/AutoHide case
        if (!slidingAnimationAutoHiddenIn.running
                && !inRelocationHiding
                && (dockView.visibility.isHidden || slidingAnimationAutoHiddenOut.running /*it is not already shown or is trying to hide*/)){
            slidingAnimationAutoHiddenIn.init();
        }
    }

    function slotMustBeHide() {
        if (root.inStartup) {
            slidingAnimationAutoHiddenOut.init();
            return;
        }

        if (inSlidingIn && !inRelocationHiding) {
            /*consider hiding after sliding in has finished*/
            return;
        }

        if (dockView && dockView.visibility.mode === LatteCore.Types.WindowsCanCover) {
            dockView.visibility.setViewOnBackLayer();
            return;
        }

        //! Normal Dodge/AutoHide case
        if (!slidingAnimationAutoHiddenOut.running
                && !dockView.visibility.blockHiding
                && (!dockView.visibility.containsMouse || dockView.visibility.mode === LatteCore.Types.SidebarOnDemand /*for SidebarOnDemand mouse should be ignored on hiding*/)
                && (!dockView.visibility.isHidden || slidingAnimationAutoHiddenIn.running /*it is not already hidden or is trying to show*/)) {
            slidingAnimationAutoHiddenOut.init();
        }
    }

    //! functions used for sliding out/in during location/screen changes
    function slotHideDockDuringLocationChange() {
        inRelocationHiding = true;

        if(!slidingAnimationAutoHiddenOut.running) {
            slidingAnimationAutoHiddenOut.init();
        }
    }

    function slotShowDockAfterLocationChange() {
        slidingAnimationAutoHiddenIn.init();
    }

    function sendHideDockDuringLocationChangeFinished(){
        dockView.positioner.hidingForRelocationFinished();
    }

    function sendSlidingOutAnimationEnded() {
        dockView.visibility.hide();
        dockView.visibility.isHidden = true;

        if (debug.maskEnabled) {
            console.log("hiding animation ended...");
        }

        sendHideDockDuringLocationChangeFinished();
    }

    ///test maskArea
    function updateMaskArea() {
        if (!dockView || !root.viewIsAvailable) {
            return;
        }

        var localX = 0;
        var localY = 0;

        // debug maskArea criteria
        if (debug.maskEnabled) {
            console.log(animations.needBothAxis.count + ", " + animations.needLength.count + ", " +
                        animations.needThickness.count + ", " + dockView.visibility.isHidden);
        }

        //console.log("reached updating geometry ::: "+dock.maskArea);


        if (!dockView.visibility.isHidden && updateIsEnabled && inNormalState) {
            //! Important: Local Geometry must not be updated when view ISHIDDEN
            //! because it breaks Dodge(s) modes in such case

            var localGeometry = Qt.rect(0, 0, root.width, root.height);

            //the shadows size must be removed from the maskArea
            //before updating the localDockGeometry
            if (!dockView.behaveAsPlasmaPanel) {
                var cleanThickness = metrics.totals.thickness;
                var edgeMargin = metrics.mask.screenEdge;

                if (plasmoid.location === PlasmaCore.Types.TopEdge) {
                    localGeometry.x = dockView.effects.rect.x; // from effects area
                    localGeometry.width = dockView.effects.rect.width; // from effects area

                    localGeometry.y = edgeMargin;
                    localGeometry.height = cleanThickness;
                } else if (plasmoid.location === PlasmaCore.Types.BottomEdge) {
                    localGeometry.x = dockView.effects.rect.x; // from effects area
                    localGeometry.width = dockView.effects.rect.width; // from effects area

                    localGeometry.y = root.height - cleanThickness - edgeMargin;
                    localGeometry.height = cleanThickness;
                } else if (plasmoid.location === PlasmaCore.Types.LeftEdge) {
                    localGeometry.y = dockView.effects.rect.y; // from effects area
                    localGeometry.height = dockView.effects.rect.height; // from effects area

                    localGeometry.x = edgeMargin;
                    localGeometry.width = cleanThickness;
                } else if (plasmoid.location === PlasmaCore.Types.RightEdge) {
                    localGeometry.y = dockView.effects.rect.y; // from effects area
                    localGeometry.height = dockView.effects.rect.height; // from effects area

                    localGeometry.x = root.width - cleanThickness - edgeMargin;
                    localGeometry.width = cleanThickness;
                }

                //set the boundaries for dockView local geometry
                //qBound = qMax(min, qMin(value, max)).

                localGeometry.x = Math.max(0, Math.min(localGeometry.x, dockView.width));
                localGeometry.y = Math.max(0, Math.min(localGeometry.y, dockView.height));
                localGeometry.width = Math.min(localGeometry.width, dockView.width);
                localGeometry.height = Math.min(localGeometry.height, dockView.height);
            }

            //console.log("update geometry ::: "+localGeometry);
            dockView.localGeometry = localGeometry;
        }

        //! Input Mask
        if (updateIsEnabled) {
            updateInputGeometry();
        }
    }

    function updateInputGeometry() {
        // VisibilityManager.qml tries to workaround faulty onEntered() signals from ParabolicMouseArea
        // by specifying inputThickness when ParabolicEffect is applied. (inputThickness->animated scenario)
        var animated = (animations.needBothAxis.count>0);

        if (!LatteCore.WindowSystem.compositingActive || dockView.behaveAsPlasmaPanel) {
            //! clear input mask
            dockView.effects.inputMask = Qt.rect(0, 0, -1, -1);
        } else {
            var floatingInternalGapAcceptsInput = behaveAsDockWithMask && floatingInternalGapIsForced;
            var inputThickness;

            if (dockView.visibility.isHidden) {
                inputThickness = metrics.mask.thickness.hidden;
            } else if (root.hasFloatingGapInputEventsDisabled) {
                inputThickness = animated ? metrics.mask.thickness.zoomedForItems - metrics.margins.screenEdge : metrics.totals.thickness;
            } else {
                inputThickness = animated ? metrics.mask.thickness.zoomedForItems : metrics.mask.screenEdge + metrics.totals.thickness;
            }

            var subtractedScreenEdge = root.hasFloatingGapInputEventsDisabled && !dockView.visibility.isHidden ? metrics.mask.screenEdge : 0;

            var inputGeometry = Qt.rect(0, 0, root.width, root.height);

            //!use view.localGeometry for length properties
            if (plasmoid.location === PlasmaCore.Types.TopEdge) {
                if (!animated) {
                    inputGeometry.x = dockView.localGeometry.x;
                    inputGeometry.width = dockView.localGeometry.width;
                }

                inputGeometry.y = subtractedScreenEdge;
                inputGeometry.height = inputThickness;
            } else if (plasmoid.location === PlasmaCore.Types.BottomEdge) {
                if (!animated) {
                    inputGeometry.x = dockView.localGeometry.x;
                    inputGeometry.width = dockView.localGeometry.width;
                }

                inputGeometry.y = root.height - inputThickness - subtractedScreenEdge;
                inputGeometry.height = inputThickness;
            } else if (plasmoid.location === PlasmaCore.Types.LeftEdge) {
                if (!animated) {
                    inputGeometry.y = dockView.localGeometry.y;
                    inputGeometry.height = dockView.localGeometry.height;
                }

                inputGeometry.x = subtractedScreenEdge;
                inputGeometry.width = inputThickness;
            } else if (plasmoid.location === PlasmaCore.Types.RightEdge) {
                if (!animated) {
                    inputGeometry.y = dockView.localGeometry.y;
                    inputGeometry.height = dockView.localGeometry.height;
                }

                inputGeometry.x = root.width - inputThickness - subtractedScreenEdge;
                inputGeometry.width = inputThickness;
            }

            //set the boundaries for dockView local geometry
            //qBound = qMax(min, qMin(value, max)).

            inputGeometry.x = Math.max(0, Math.min(inputGeometry.x, dockView.width));
            inputGeometry.y = Math.max(0, Math.min(inputGeometry.y, dockView.height));
            inputGeometry.width = Math.min(inputGeometry.width, dockView.width);
            inputGeometry.height = Math.min(inputGeometry.height, dockView.height);

            if (dockView.visibility.isSidebar && dockView.visibility.isHidden) {
                //! this way we make sure than no input is accepted anywhere
                inputGeometry = Qt.rect(-1, -1, 1, 1);
            }

            dockView.effects.inputMask = inputGeometry;
        }
    }

    Loader{
        anchors.fill: parent
        active: debug.graphicsEnabled

        sourceComponent: Item{
            anchors.fill:parent

            Rectangle{
                id: windowBackground
                anchors.fill: parent
                border.color: "red"
                border.width: 1
                color: "transparent"
            }

            Rectangle{
                x: dockView ? dockView.effects.mask.x : -1
                y: dockView ? dockView.effects.mask.y : -1
                height: dockView ? dockView.effects.mask.height : 0
                width: dockView ? dockView.effects.mask.width : 0

                border.color: "green"
                border.width: 1
                color: "transparent"
            }
        }
    }

    /***Hiding/Showing Animations*****/

    //////////////// Animations - Slide In - Out
    SequentialAnimation{
        id: slidingAnimationAutoHiddenOut

        PropertyAnimation {
            target: !root.behaveAsPlasmaPanel ? layoutsContainer : dockView.positioner
            property: !root.behaveAsPlasmaPanel ? (root.isVertical ? "x" : "y") : "slideOffset"
            to: {
                if (root.behaveAsPlasmaPanel) {
                    return slidingOutToPos;
                }

                if (LatteCore.WindowSystem.compositingActive) {
                    return slidingOutToPos;
                } else {
                    if ((plasmoid.location===PlasmaCore.Types.LeftEdge)||(plasmoid.location===PlasmaCore.Types.TopEdge)) {
                        return slidingOutToPos + 1;
                    } else {
                        return slidingOutToPos - 1;
                    }
                }
            }
            duration: manager.animationSpeed
            easing.type: Easing.InQuad
        }

        ScriptAction{
            script: {
                dockView.visibility.isHidden = true;

                if (root.behaveAsPlasmaPanel && dockView.positioner.slideOffset !== 0) {
                    //! hide real panels when they slide-out
                    dockView.visibility.hide();
                }
            }
        }

        onStarted: {
            if (debug.maskEnabled) {
                console.log("hiding animation started...");
            }
        }

        onStopped: {
            //! Trying to move the ending part of the signals at the end of editing animation
            if (!manager.inRelocationHiding) {
                manager.updateMaskArea();
            } else {
                if (!root.editMode) {
                    manager.sendSlidingOutAnimationEnded();
                }
            }

            dockView.visibility.slideOutFinished();
            manager.updateInputGeometry();

            if (root.inStartup) {
                //! when view is first created slide-outs when that animation ends then
                //! it flags that startup has ended and first slide-in can be started
                //! this is important because if it is removed then some views
                //! wont slide-in after startup.
                root.inStartup = false;
            }
        }

        function init() {
            if (manager.inRelocationAnimation || root.inStartup/*used from recreating views*/ || !dockView.visibility.blockHiding) {
                start();
            }
        }
    }

    SequentialAnimation{
        id: slidingAnimationAutoHiddenIn

        PauseAnimation{
            duration: manager.inRelocationHiding && animations.active ? 500 : 0
        }

        PropertyAnimation {
            target: !root.behaveAsPlasmaPanel ? layoutsContainer : dockView.positioner
            property: !root.behaveAsPlasmaPanel ? (root.isVertical ? "x" : "y") : "slideOffset"
            to: 0
            duration: manager.animationSpeed
            easing.type: Easing.OutQuad
        }

        ScriptAction{
            script: {
                // deprecated
                // root.inStartup = false;
            }
        }

        onStarted: {
            dockView.visibility.show();
            manager.updateInputGeometry();

            if (debug.maskEnabled) {
                console.log("showing animation started...");
            }
        }

        onStopped: {
            inSlidingIn = false;

            if (manager.inRelocationHiding) {
                manager.inRelocationHiding = false;
                autosize.updateIconSize();
            }

            manager.inRelocationHiding = false;
            autosize.updateIconSize();

            if (debug.maskEnabled) {
                console.log("showing animation ended...");
            }

            dockView.visibility.slideInFinished();

            //! this is needed in order to update dock absolute geometry correctly in the end AND
            //! when a floating dock is sliding-in through masking techniques
            updateMaskArea();
        }

        function init() {
            if (!root.viewIsAvailable) {
                return;
            }

            inSlidingIn = true;

            if (slidingAnimationAutoHiddenOut.running) {
                slidingAnimationAutoHiddenOut.stop();
            }

            dockView.visibility.isHidden = false;
            updateMaskArea();

            start();
        }
    }

    //! Slides Animations for FLOATING+BEHAVEASPLASMAPANEL when
    //! HIDETHICKSCREENCAP dynamically is enabled/disabled
    SequentialAnimation{
        id: slidingInRealFloating

        PropertyAnimation {
            target: dockView ? dockView.positioner : null
            property: "slideOffset"
            to: 0
            duration: manager.animationSpeed
            easing.type: Easing.OutQuad
        }

        ScriptAction{
            script: {
                dockView.positioner.inSlideAnimation = false;
            }
        }

        onStopped: dockView.positioner.inSlideAnimation = false;

    }

    SequentialAnimation{
        id: slidingOutRealFloating

        ScriptAction{
            script: {
                dockView.positioner.inSlideAnimation = true;
            }
        }

        PropertyAnimation {
            target: dockView ? dockView.positioner : null
            property: "slideOffset"
            to: plasmoid.configuration.screenEdgeMargin
            duration: manager.animationSpeed
            easing.type: Easing.InQuad
        }
    }

    Connections {
        target: root
        onHideThickScreenGapChanged: {
            if (!dockView || !root.viewIsAvailable) {
                return;
            }

            if (root.behaveAsPlasmaPanel && !dockView.visibility.isHidden && !inSlidingIn && !inSlidingOut && !inStartup) {
                slideInOutRealFloating();
            }
        }

        onInStartupChanged: {
            //! used for positioning properly real floating panels when there is a maximized window
            if (root.hideThickScreenGap && !inStartup && dockView.positioner.slideOffset===0) {
                if (root.behaveAsPlasmaPanel && !dockView.visibility.isHidden) {
                    slideInOutRealFloating();
                }
            }
        }

        function slideInOutRealFloating() {
            if (root.hideThickScreenGap) {
                slidingInRealFloating.stop();
                slidingOutRealFloating.start();
            } else {
                slidingOutRealFloating.stop();
                slidingInRealFloating.start();
            }
        }
    }


}
