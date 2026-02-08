/* This file is a part of the Atmo desktop experience's SynDock project for SynOS .
 * Copyright (C) 2026 Syndromatic Ltd. All rights reserved
 * Designed by Kavish Krishnakumar in Manchester.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or 
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITH ABSOLUTELY NO WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Based on Latte Dock:
 * SPDX-FileCopyrightText: 2016 Smith AR <audoban@openmailbox.org>
 * SPDX-FileCopyrightText: 2016 Michail Vourlakos <mvourlakos@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "nsewaylandinterface.h"
#include "../lattecorona.h"
#include "../view/view.h"
#include "../view/positioner.h"

// Qt
#include <QDebug>
#include <QGuiApplication>
#include <QTimer>

// KDE Frameworks 6
#include <KWindowSystem>
#include <KWindowEffects>

Q_LOGGING_CATEGORY(nseWayland, "nse.wayland", QtInfoMsg)

namespace NSE {
namespace WindowSystem {

// =============================================================================
// Construction / Destruction
// =============================================================================

NSEWaylandInterface::NSEWaylandInterface(QObject *parent)
    : NSE::WindowSystem::AbstractWindowInterface(parent)
{
    m_corona = qobject_cast<NSE::Corona*>(parent);
    
    if (!m_corona) {
        setError("Constructor", "Parent is not a valid Corona object");
    }
    
    qCInfo(nseWayland) << "NSE Wayland Interface constructed (KWin 6.x only)";
}

NSEWaylandInterface::~NSEWaylandInterface()
{
    // Clean up LayerShell windows
    for (auto it = m_layerWindows.begin(); it != m_layerWindows.end(); ++it) {
        // LayerShellQt::Window is owned by the QWindow, no explicit delete needed
    }
    m_layerWindows.clear();
    
    qCInfo(nseWayland) << "NSE Wayland Interface destroyed";
}

// =============================================================================
// Initialisation
// =============================================================================

bool NSEWaylandInterface::init()
{
    if (m_initialised) {
        qCWarning(nseWayland) << "Already initialised, skipping";
        return true;
    }
    
    // Verify we're running on Wayland
    if (!QGuiApplication::platformName().contains(QLatin1String("wayland"))) {
        setError("init", "SynDock requires a Wayland session. Current platform: " 
                 + QGuiApplication::platformName());
        emit compositorError(m_lastError);
        return false;
    }
    
    // Initialise LayerShellQt
    LayerShellQt::Shell::useLayerShell();
    
    // Verify LayerShell is available
    // Note: LayerShellQt doesn't have an explicit availability check,
    // but we can verify by checking the platform
    if (!KWindowSystem::isPlatformWayland()) {
        setError("init", "KWindowSystem reports non-Wayland platform");
        emit compositorError(m_lastError);
        return false;
    }
    
    m_initialised = true;
    qCInfo(nseWayland) << "LayerShellQt initialised successfully for KWin 6.x";
    
    emit layerShellAvailabilityChanged(true);
    return true;
}

// =============================================================================
// LayerShell Configuration
// =============================================================================

bool NSEWaylandInterface::configurePanelSurface(QWindow *window, 
                                                 NSE::DockEdge edge,
                                                 NSE::Visibility visibility)
{
    if (!window) {
        setError("configurePanelSurface", "Null window pointer");
        return false;
    }
    
    if (!m_initialised) {
        setError("configurePanelSurface", "Interface not initialised");
        return false;
    }
    
    LayerShellQt::Window *layerWindow = getLayerWindow(window);
    if (!layerWindow) {
        setError("configurePanelSurface", 
                 QString("Failed to get LayerShell window for %1")
                     .arg(window->title()));
        return false;
    }
    
    // Configure anchors based on edge
    layerWindow->setAnchors(edgeToAnchors(edge));
    
    // Configure layer based on visibility mode
    layerWindow->setLayer(visibilityToLayer(visibility));
    
    // Panel-specific settings
    layerWindow->setKeyboardInteractivity(
        LayerShellQt::Window::KeyboardInteractivityOnDemand);
    
    // Set scope for identification
    layerWindow->setScope(QStringLiteral("syndock-panel"));
    
    qCDebug(nseWayland) << "Configured panel surface:" 
                        << "edge=" << NSE::dockEdgeToString(edge)
                        << "visibility=" << NSE::visibilityToString(visibility)
                        << "window=" << window->title();
    
    return true;
}

void NSEWaylandInterface::setExclusiveZone(QWindow *window, int size)
{
    if (!window) {
        qCWarning(nseWayland) << "setExclusiveZone: null window";
        return;
    }
    
    LayerShellQt::Window *layerWindow = getLayerWindow(window);
    if (layerWindow) {
        layerWindow->setExclusiveZone(size);
        qCDebug(nseWayland) << "Set exclusive zone:" << size << "px for" << window->title();
    }
}

void NSEWaylandInterface::clearExclusiveZone(QWindow *window)
{
    setExclusiveZone(window, 0);
}

// =============================================================================
// AbstractWindowInterface Overrides
// =============================================================================

void NSEWaylandInterface::setViewExtraFlags(QObject *view, bool isPanelWindow,
                                            NSE::Types::Visibility mode)
{
    NSE::View *latteView = qobject_cast<NSE::View*>(view);
    
    if (!latteView) {
        qCWarning(nseWayland) << "setViewExtraFlags: view is not a NSE::View";
        return;
    }
    
    QWindow *window = latteView;
    if (!window) {
        qCWarning(nseWayland) << "setViewExtraFlags: failed to get QWindow";
        return;
    }
    
    // Convert Latte visibility to NSE visibility
    NSE::Visibility nseVisibility = NSE::Visibility::AlwaysVisible;
    switch (mode) {
        case NSE::Types::AutoHide:
            nseVisibility = NSE::Visibility::AutoHide;
            break;
        case NSE::Types::DodgeActive:
            nseVisibility = NSE::Visibility::DodgeActive;
            break;
        case NSE::Types::DodgeMaximized:
            nseVisibility = NSE::Visibility::DodgeMaximised;
            break;
        case NSE::Types::DodgeAllWindows:
            nseVisibility = NSE::Visibility::DodgeAllWindows;
            break;
        case NSE::Types::WindowsGoBelow:
            nseVisibility = NSE::Visibility::WindowsGoBelow;
            break;
        case NSE::Types::WindowsCanCover:
            nseVisibility = NSE::Visibility::WindowsCanCover;
            break;
        case NSE::Types::WindowsAlwaysCover:
            nseVisibility = NSE::Visibility::WindowsAlwaysCover;
            break;
        case NSE::Types::SideBar:
            nseVisibility = NSE::Visibility::SideBar;
            break;
        default:
            nseVisibility = NSE::Visibility::AlwaysVisible;
            break;
    }
    
    // Get edge from view positioner
    NSE::DockEdge edge = NSE::DockEdge::Bottom;
    if (latteView->positioner()) {
        switch (latteView->location()) {
            case Plasma::Types::TopEdge:
                edge = NSE::DockEdge::Top;
                break;
            case Plasma::Types::BottomEdge:
                edge = NSE::DockEdge::Bottom;
                break;
            case Plasma::Types::LeftEdge:
                edge = NSE::DockEdge::Left;
                break;
            case Plasma::Types::RightEdge:
                edge = NSE::DockEdge::Right;
                break;
            default:
                edge = NSE::DockEdge::Bottom;
                break;
        }
    }
    
    if (isPanelWindow) {
        configurePanelSurface(window, edge, nseVisibility);
    }
}

void NSEWaylandInterface::setViewStruts(QWindow &view, const QRect &rect,
                                         Plasma::Types::Location location)
{
    // Calculate exclusive zone based on location and rect
    int exclusiveSize = 0;
    
    switch (location) {
        case Plasma::Types::TopEdge:
        case Plasma::Types::BottomEdge:
            exclusiveSize = rect.height();
            break;
        case Plasma::Types::LeftEdge:
        case Plasma::Types::RightEdge:
            exclusiveSize = rect.width();
            break;
        default:
            exclusiveSize = 0;
            break;
    }
    
    setExclusiveZone(&view, exclusiveSize);
}

void NSEWaylandInterface::removeViewStruts(QWindow &view)
{
    clearExclusiveZone(&view);
}

WindowId NSEWaylandInterface::activeWindow()
{
    // TODO: Implement using KWindowSystem for KF6
    // KWindowSystem::activeWindow() works on Wayland in KF6
    return WindowId();
}

void NSEWaylandInterface::skipTaskBar(const QDialog &dialog)
{
    // In KF6/Wayland, this is handled via window hints
    // The PlasmaShellSurface approach is deprecated
    qCDebug(nseWayland) << "skipTaskBar: using window hints for dialog";
}

void NSEWaylandInterface::slideWindow(QWindow &view, Slide location)
{
    auto slideLocation = KWindowEffects::NoEdge;
    
    switch (location) {
        case Slide::Top:
            slideLocation = KWindowEffects::TopEdge;
            break;
        case Slide::Bottom:
            slideLocation = KWindowEffects::BottomEdge;
            break;
        case Slide::Left:
            slideLocation = KWindowEffects::LeftEdge;
            break;
        case Slide::Right:
            slideLocation = KWindowEffects::RightEdge;
            break;
        default:
            break;
    }
    
    KWindowEffects::slideWindow(&view, slideLocation);
}

void NSEWaylandInterface::enableBlurBehind(QWindow &view)
{
    KWindowEffects::enableBlurBehind(&view, true);
}

void NSEWaylandInterface::setActiveEdge(QWindow *view, bool active)
{
    // Auto-hide edge activation for LayerShell
    if (!view) return;
    
    LayerShellQt::Window *layerWindow = getLayerWindow(view);
    if (layerWindow) {
        if (active) {
            // Show panel when edge is active
            layerWindow->setExclusiveZone(-1); // Auto-exclusive
        } else {
            layerWindow->setExclusiveZone(0);
        }
    }
}

void NSEWaylandInterface::setFrameExtents(QWindow *view, const QMargins &extents)
{
    Q_UNUSED(view)
    Q_UNUSED(extents)
    // Frame extents not directly supported in LayerShell
    // Margins are handled differently in Wayland
}

void NSEWaylandInterface::setInputMask(QWindow *window, const QRect &rect)
{
    if (window) {
        window->setMask(QRegion(rect));
    }
}

WindowInfoWrap NSEWaylandInterface::requestInfoActive()
{
    // TODO: Implement using KWindowSystem for KF6
    return WindowInfoWrap();
}

WindowInfoWrap NSEWaylandInterface::requestInfo(WindowId wid)
{
    Q_UNUSED(wid)
    // TODO: Implement using KWindowSystem for KF6
    return WindowInfoWrap();
}

AppData NSEWaylandInterface::appDataFor(WindowId wid)
{
    Q_UNUSED(wid)
    // TODO: Implement using task manager
    return AppData();
}

QIcon NSEWaylandInterface::iconFor(WindowId wid)
{
    Q_UNUSED(wid)
    // TODO: Implement using task manager
    return QIcon();
}

WindowId NSEWaylandInterface::winIdFor(QString appId, QString title)
{
    Q_UNUSED(appId)
    Q_UNUSED(title)
    // TODO: Implement window search
    return WindowId();
}

WindowId NSEWaylandInterface::winIdFor(QString appId, QRect geometry)
{
    Q_UNUSED(appId)
    Q_UNUSED(geometry)
    // TODO: Implement window search
    return WindowId();
}

bool NSEWaylandInterface::windowCanBeDragged(WindowId wid)
{
    Q_UNUSED(wid)
    // TODO: Implement with window info
    return true;
}

bool NSEWaylandInterface::windowCanBeMaximized(WindowId wid)
{
    Q_UNUSED(wid)
    // TODO: Implement with window info
    return true;
}

void NSEWaylandInterface::requestActivate(WindowId wid)
{
    Q_UNUSED(wid)
    // TODO: Implement using KWindowSystem
}

void NSEWaylandInterface::requestClose(WindowId wid)
{
    Q_UNUSED(wid)
    // TODO: Implement using task manager
}

void NSEWaylandInterface::requestMoveWindow(WindowId wid, QPoint from)
{
    Q_UNUSED(wid)
    Q_UNUSED(from)
    // TODO: Implement window move
}

void NSEWaylandInterface::requestToggleIsOnAllDesktops(WindowId wid)
{
    Q_UNUSED(wid)
    // TODO: Implement desktop toggle
}

void NSEWaylandInterface::requestToggleKeepAbove(WindowId wid)
{
    Q_UNUSED(wid)
    // TODO: Implement keep above toggle
}

void NSEWaylandInterface::setKeepAbove(WindowId wid, bool active)
{
    Q_UNUSED(wid)
    Q_UNUSED(active)
    // TODO: Implement keep above
}

void NSEWaylandInterface::setKeepBelow(WindowId wid, bool active)
{
    Q_UNUSED(wid)
    Q_UNUSED(active)
    // TODO: Implement keep below
}

void NSEWaylandInterface::requestToggleMinimized(WindowId wid)
{
    Q_UNUSED(wid)
    // TODO: Implement minimize toggle
}

void NSEWaylandInterface::requestToggleMaximized(WindowId wid)
{
    Q_UNUSED(wid)
    // TODO: Implement maximize toggle
}

void NSEWaylandInterface::setWindowOnActivities(const WindowId &wid,
                                                 const QStringList &activities)
{
    Q_UNUSED(wid)
    Q_UNUSED(activities)
    // TODO: Implement activity assignment
}

void NSEWaylandInterface::switchToNextVirtualDesktop()
{
    // TODO: Implement using KWindowSystem
    qCDebug(nseWayland) << "switchToNextVirtualDesktop";
}

void NSEWaylandInterface::switchToPreviousVirtualDesktop()
{
    // TODO: Implement using KWindowSystem
    qCDebug(nseWayland) << "switchToPreviousVirtualDesktop";
}

void NSEWaylandInterface::registerIgnoredWindow(WindowId wid)
{
    if (!wid.isNull() && !m_ignoredWindows.contains(wid)) {
        m_ignoredWindows.append(wid);
        emit windowChanged(wid);
    }
}

void NSEWaylandInterface::unregisterIgnoredWindow(WindowId wid)
{
    if (m_ignoredWindows.contains(wid)) {
        m_ignoredWindows.removeAll(wid);
        emit windowRemoved(wid);
    }
}

// =============================================================================
// Internal Helpers
// =============================================================================

LayerShellQt::Window::Anchors NSEWaylandInterface::edgeToAnchors(NSE::DockEdge edge) const
{
    using Anchor = LayerShellQt::Window::Anchor;
    
    switch (edge) {
        case NSE::DockEdge::Bottom:
            return Anchor::AnchorBottom | Anchor::AnchorLeft | Anchor::AnchorRight;
        case NSE::DockEdge::Top:
            return Anchor::AnchorTop | Anchor::AnchorLeft | Anchor::AnchorRight;
        case NSE::DockEdge::Left:
            return Anchor::AnchorLeft | Anchor::AnchorTop | Anchor::AnchorBottom;
        case NSE::DockEdge::Right:
            return Anchor::AnchorRight | Anchor::AnchorTop | Anchor::AnchorBottom;
        case NSE::DockEdge::Centre:
            // Floating dock - no anchors, positioned in centre
            return {};
    }
    
    return Anchor::AnchorBottom | Anchor::AnchorLeft | Anchor::AnchorRight;
}

LayerShellQt::Window::Layer NSEWaylandInterface::visibilityToLayer(
    NSE::Visibility visibility) const
{
    using Layer = LayerShellQt::Window::Layer;
    
    switch (visibility) {
        case NSE::Visibility::AlwaysVisible:
        case NSE::Visibility::AutoHide:
        case NSE::Visibility::DodgeActive:
        case NSE::Visibility::DodgeMaximised:
        case NSE::Visibility::DodgeAllWindows:
            // Panels typically use the Top layer
            return Layer::LayerTop;
            
        case NSE::Visibility::WindowsGoBelow:
            // Panel above windows
            return Layer::LayerOverlay;
            
        case NSE::Visibility::WindowsCanCover:
        case NSE::Visibility::WindowsAlwaysCover:
            // Panel below windows
            return Layer::LayerBottom;
            
        case NSE::Visibility::SideBar:
            return Layer::LayerTop;
    }
    
    return Layer::LayerTop;
}

LayerShellQt::Window* NSEWaylandInterface::getLayerWindow(QWindow *window)
{
    if (!window) {
        return nullptr;
    }
    
    // Check cache first
    if (m_layerWindows.contains(window)) {
        return m_layerWindows.value(window);
    }
    
    // Create new LayerShell window
    LayerShellQt::Window *layerWindow = LayerShellQt::Window::get(window);
    
    if (layerWindow) {
        m_layerWindows.insert(window, layerWindow);
        
        // Clean up when window is destroyed
        connect(window, &QObject::destroyed, this, [this, window]() {
            m_layerWindows.remove(window);
        });
    } else {
        qCWarning(nseWayland) << "Failed to create LayerShell window for:" 
                              << window->title();
    }
    
    return layerWindow;
}

void NSEWaylandInterface::setError(const QString &context, const QString &message)
{
    m_lastError = QString("[%1] %2").arg(context, message);
    qCCritical(nseWayland) << m_lastError;
}

} // namespace WindowSystem
} // namespace NSE

#include "nsewaylandinterface.moc"
