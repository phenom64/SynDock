/*
    SPDX-FileCopyrightText: 2021 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid

import org.kde.syndock.core 0.2 as LatteCore
import org.kde.syndock.private.containment 0.1 as LatteContainment

Item {
    property bool updateIsEnabled: autosize.inCalculatedIconSize
                                   && !visibilityManager.inSlidingIn
                                   && !visibilityManager.inSlidingOut
                                   && !visibilityManager.inRelocationHiding

    //! Latte::View Main Bindings 
    Binding{
        target: dockView
        property:"maxThickness"
        //! prevents updating window geometry during closing window in wayland and such fixes a crash
        when: dockView && !visibilityManager.inRelocationHiding && !visibilityManager.inClientSideScreenEdgeSliding //&& !inStartup
        value: root.behaveAsPlasmaPanel ? visibilityManager.thicknessAsPanel : metrics.maxThicknessForView
    }

    Binding{
        target: dockView
        property:"normalThickness"
        when: dockView && updateIsEnabled
        value: root.behaveAsPlasmaPanel ? visibilityManager.thicknessAsPanel : metrics.mask.screenEdge + metrics.mask.thickness.maxNormalForItemsWithoutScreenEdge
    }

    Binding{
        target: dockView
        property:"maxNormalThickness"
        when: dockView && updateIsEnabled
        value: metrics.mask.thickness.maxNormal
    }

    Binding {
        target: dockView
        property: "headThicknessGap"
        when: dockView && updateIsEnabled && !visibilityManager.inClientSideScreenEdgeSliding
        value: {
            if (root.behaveAsPlasmaPanel || root.viewType === LatteCore.Types.PanelView || (dockView && dockView.byPassWM)) {
                return 0;
            }

            return metrics.maxThicknessForView - metrics.mask.thickness.maxNormalForItems;
        }
    }

    Binding{
        target: dockView
        property: "type"
        when: dockView
        value: root.viewType
    }

    Binding{
        target: dockView
        property: "behaveAsPlasmaPanel"
        when: dockView
        value: root.behaveAsPlasmaPanel
    }

    Binding{
        target: dockView
        property: "fontPixelSize"
        when: theme
        value: theme.defaultFont.pixelSize
    }

    Binding{
        target: dockView
        property: "maxLength"
        when: dockView
        value: root.maxLengthPerCentage/100
    }

    Binding{
        target: dockView
        property: "offset"
        when: dockView
        value: plasmoid.configuration.offset/100
    }

    Binding{
        target: dockView
        property: "screenEdgeMargin"
        when: dockView
        value: Math.max(0, plasmoid.configuration.screenEdgeMargin)
    }

    Binding{
        target: dockView
        property: "screenEdgeMarginEnabled"
        when: dockView
        value: root.screenEdgeMarginEnabled && !root.hideThickScreenGap
    }

    Binding{
        target: dockView
        property: "alignment"
        when: dockView
        value: myView.alignment
    }

    Binding{
        target: dockView
        property: "isTouchingTopViewAndIsBusy"
        when: root.viewIsAvailable
        value: {
            if (!root.viewIsAvailable) {
                return false;
            }

            var isTouchingTopScreenEdge = (dockView.y === dockView.screenGeometry.y);
            var isStickedOnTopBorder = (plasmoid.configuration.alignment === LatteCore.Types.Justify && plasmoid.configuration.maxLength===100)
                    || (plasmoid.configuration.alignment === LatteCore.Types.Top && plasmoid.configuration.offset===0);

            return root.isVertical && !dockView.visibility.isHidden && !isTouchingTopScreenEdge && isStickedOnTopBorder && background.isShown;
        }
    }

    Binding{
        target: dockView
        property: "isTouchingBottomViewAndIsBusy"
        when: dockView
        value: {
            if (!root.viewIsAvailable) {
                return false;
            }

            var latteBottom = dockView.y + dockView.height;
            var screenBottom = dockView.screenGeometry.y + dockView.screenGeometry.height;
            var isTouchingBottomScreenEdge = (latteBottom === screenBottom);

            var isStickedOnBottomBorder = (plasmoid.configuration.alignment === LatteCore.Types.Justify && plasmoid.configuration.maxLength===100)
                    || (plasmoid.configuration.alignment === LatteCore.Types.Bottom && plasmoid.configuration.offset===0);

            return root.isVertical && !dockView.visibility.isHidden && !isTouchingBottomScreenEdge && isStickedOnBottomBorder && background.isShown;
        }
    }

    Binding{
        target: dockView
        property: "colorizer"
        when: dockView
        value: colorizerManager
    }

    Binding{
        target: dockView
        property: "metrics"
        when: dockView
        value: metrics
    }

    //! View::Effects bindings
    Binding{
        target: dockView && dockView.effects ? dockView.effects : null
        property: "backgroundAllCorners"
        when: dockView && dockView.effects
        value: plasmoid.configuration.backgroundAllCorners
               && (!root.screenEdgeMarginEnabled /*no-floating*/
                   || (root.screenEdgeMarginEnabled /*floating with justify alignment and 100% maxlength*/
                       && plasmoid.configuration.maxLength===100
                       && myView.alignment===LatteCore.Types.Justify
                       && !root.hideLengthScreenGaps))
    }

    Binding{
        target: dockView && dockView.effects ? dockView.effects : null
        property: "backgroundRadius"
        when: dockView && dockView.effects
        value: background.customRadius
    }

    Binding{
        target: dockView && dockView.effects ? dockView.effects : null
        property: "backgroundRadiusEnabled"
        when: dockView && dockView.effects
        value: background.customRadiusIsEnabled
    }

    Binding{
        target: dockView && dockView.effects ? dockView.effects : null
        property: "backgroundOpacity"
        when: dockView && dockView.effects
        value: plasmoid.configuration.panelTransparency===-1 /*Default option*/ ? -1 : background.currentOpacity
    }

    Binding{
        target: dockView && dockView.effects ? dockView.effects : null
        property: "drawEffects"
        when: dockView && dockView.effects && !root.inStartup
        value: LatteCore.WindowSystem.compositingActive
               && (((root.blurEnabled && root.useThemePanel) || (root.blurEnabled && root.forceSolidPanel))
                   && (!root.inStartup || visibilityManager.inRelocationHiding))
    }

    Binding{
        target: dockView && dockView.effects ? dockView.effects : null
        property: "drawShadows"
        when: dockView && dockView.effects
        value: root.drawShadowsExternal && (!root.inStartup || visibilityManager.inRelocationHiding) && !(dockView && dockView.visibility.isHidden)
    }

    Binding{
        target: dockView && dockView.effects ? dockView.effects : null
        property:"editShadow"
        when: dockView && dockView.effects
        value: root.editShadow
    }

    Binding{
        target: dockView && dockView.effects ? dockView.effects : null
        property:"innerShadow"
        when: dockView && dockView.effects
        value: background.shadows.headThickness
    }

    Binding{
        target: dockView && dockView.effects ? dockView.effects : null
        property: "panelBackgroundSvg"
        when: dockView && dockView.effects
        value: background.panelBackgroundSvg
    }

    Binding{
        target: dockView && dockView.effects ? dockView.effects : null
        property:"appletsLayoutGeometry"
        when: dockView && dockView.effects && visibilityManager.inNormalState
        value: {
            if (root.behaveAsPlasmaPanel
                    || !LatteCore.WindowSystem.compositingActive
                    || (!parabolic.isEnabled && root.userShowPanelBackground && plasmoid.configuration.panelSize===100)) {
                var paddingtail = background.tailRoundness + background.tailRoundnessMargin;
                var paddinghead = background.headRoundness + background.headRoundnessMargin;

                if (root.isHorizontal) {
                    return Qt.rect(dockView.localGeometry.x + paddingtail,
                                   dockView.localGeometry.y,
                                   dockView.localGeometry.width - paddingtail - paddinghead,
                                   dockView.localGeometry.height);
                } else {
                    return Qt.rect(dockView.localGeometry.x,
                                   dockView.localGeometry.y + paddingtail,
                                   dockView.localGeometry.width,
                                   dockView.localGeometry.height - paddingtail - paddinghead);
                }
            }

            return Qt.rect(-1, -1, 0, 0);
        }
    }

    //! View::Positioner bindings
    Binding{
        target: dockView && dockView.positioner ? dockView.positioner : null
        property: "isStickedOnTopEdge"
        when: dockView && dockView.positioner
        value: plasmoid.configuration.isStickedOnTopEdge
    }

    Binding{
        target: dockView && dockView.positioner ? dockView.positioner : null
        property: "isStickedOnBottomEdge"
        when: dockView && dockView.positioner
        value: plasmoid.configuration.isStickedOnBottomEdge
    }

    //! View::VisibilityManager
    Binding{
        target: dockView && dockView.visibility ? dockView.visibility : null
        property: "isShownFully"
        when: dockView && dockView.visibility
        value: myView.isShownFully
    }

    Binding{
        target: dockView && dockView.visibility ? dockView.visibility : null
        property: "strutsThickness"
        when: dockView && dockView.visibility
        value: {
            var isCapableToHideScreenGap = root.screenEdgeMarginEnabled && plasmoid.configuration.hideFloatingGapForMaximized
            var mirrorGapFactor = root.mirrorScreenGap ? 2 : 1;

            //! Hide Thickness Screen Gap scenario provides two different struts thicknesses.
            //! [1] The first struts thickness is when there is no maximized window and is such case
            //!     the view is behaving as in normal AlwaysVisible visibility mode. This is very useful
            //!     when users tile windows. [bug #432122]
            //! [2] The second struts thickness is when there is a maximized window present and in such case
            //!     the view is hiding all of its screen edges. It is used mostly when the view is wanted
            //!     to act as a window titlebar.
            var thicknessForIsCapableToHideScreenGap = (root.hideThickScreenGap ? 0 : mirrorGapFactor * metrics.mask.screenEdge);

            if (root.behaveAsPlasmaPanel) {
                return isCapableToHideScreenGap ?
                            (visibilityManager.thicknessAsPanel + thicknessForIsCapableToHideScreenGap) :
                            (mirrorGapFactor*metrics.mask.screenEdge) + visibilityManager.thicknessAsPanel;
            }

            var edgeThickness = isCapableToHideScreenGap ? thicknessForIsCapableToHideScreenGap : metrics.mask.screenEdge * mirrorGapFactor;
            return edgeThickness + metrics.mask.thickness.maxNormalForItemsWithoutScreenEdge;
        }
    }

    Binding {
        target: dockView && dockView.visibility ? dockView.visibility : null
        property: "isFloatingGapWindowEnabled"
        when: dockView && dockView.visibility
        value: root.hasFloatingGapInputEventsDisabled
               && (dockView.visibility.mode === LatteCore.Types.AutoHide
                   || dockView.visibility.mode === LatteCore.Types.DodgeActive
                   || dockView.visibility.mode === LatteCore.Types.DodgeAllWindows
                   || dockView.visibility.mode === LatteCore.Types.DodgeMaximized
                   || dockView.visibility.mode === LatteCore.Types.SidebarAutoHide)
    }

    //! View::WindowsTracker bindings
    Binding{
        target: dockView && dockView.windowsTracker ? dockView.windowsTracker : null
        property: "enabled"
        //! During startup phase windows tracking is not enabled and does not
        //! influence startup sequence at all. At the same time no windows tracking
        //! takes place during startup and as such startup time is reduced
        when: dockView && dockView.windowsTracker && dockView.visibility && !root.inStartup
        value: (dockView && dockView.visibility
                && !(dockView.visibility.mode === LatteCore.Types.AlwaysVisible /* Visibility */
                     || dockView.visibility.mode === LatteCore.Types.WindowsGoBelow
                     || dockView.visibility.mode === LatteCore.Types.AutoHide))
               || indexer.clientsTrackingWindowsCount  > 0                   /*Applets Need Windows Tracking */
               || root.dragActiveWindowEnabled                               /*Dragging Active Window(Empty Areas)*/
               || ((root.backgroundOnlyOnMaximized                           /*Dynamic Background */
                    || plasmoid.configuration.solidBackgroundForMaximized
                    || root.disablePanelShadowMaximized
                    || root.windowColors !== LatteContainment.Types.NoneWindowColors))
               || (root.screenEdgeMarginsEnabled                             /*Dynamic Screen Edge Margin*/
                   && plasmoid.configuration.hideFloatingGapForMaximized)
    }

    //! View::ExtendedInterface bindings
    Binding{
        target: dockView && dockView.extendedInterface ? dockView.extendedInterface : null
        property: "plasmoid"
        when: dockView && dockView.extendedInterface
        value: plasmoid
    }

    Binding{
        target: dockView && dockView.extendedInterface ? dockView.extendedInterface : null
        property: "layoutManager"
        when: dockView && dockView.extendedInterface
        value: fastLayoutManager
    }
}
