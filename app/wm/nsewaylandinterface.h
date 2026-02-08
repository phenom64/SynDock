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

#ifndef NSEWAYLANDINTERFACE_H
#define NSEWAYLANDINTERFACE_H

#include "abstractwindowinterface.h"
#include "../NSETypes.h"

// Qt
#include <QObject>
#include <QWindow>
#include <QHash>
#include <QPointer>
#include <QLoggingCategory>

// LayerShellQt (KWin 6.x)
#include <LayerShellQt/shell.h>
#include <LayerShellQt/window.h>

// KDE Frameworks 6
#include <KWindowSystem>

Q_DECLARE_LOGGING_CATEGORY(nseWayland)

namespace NSE {
    class Corona;
}

namespace NSE {
namespace WindowSystem {

/**
 * @brief NSE Wayland Interface for KWin 6.x integration.
 * 
 * This class provides first-class Wayland support using LayerShellQt
 * for panel surface management. It replaces the deprecated KWayland::Client
 * APIs with modern Plasma 6 equivalents.
 * 
 * Features:
 * - LayerShell surface configuration for dock panels
 * - Exclusive zone management for struts
 * - Comprehensive error handling with logging
 * - Deep integration with KWin 6.x compositor
 * 
 * @note X11 support has been removed. This interface is Wayland-only.
 */
class NSEWaylandInterface : public NSE::WindowSystem::AbstractWindowInterface
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the NSE Wayland interface.
     * @param parent The parent Corona object
     */
    explicit NSEWaylandInterface(QObject *parent = nullptr);
    
    /**
     * @brief Destructor - cleans up LayerShell resources.
     */
    ~NSEWaylandInterface() override;

    // =========================================================================
    // Initialisation
    // =========================================================================
    
    /**
     * @brief Initialises the Wayland interface.
     * 
     * Sets up LayerShellQt and connects to compositor signals.
     * Must be called after construction.
     * 
     * @return true if initialisation succeeded, false otherwise
     */
    [[nodiscard]] bool init();
    
    /**
     * @brief Checks if the interface is fully initialised.
     * @return true if LayerShell is available and connected
     */
    [[nodiscard]] bool isInitialised() const { return m_initialised; }
    
    /**
     * @brief Gets the last error message if initialisation failed.
     * @return Error message string, empty if no error
     */
    [[nodiscard]] QString lastError() const { return m_lastError; }

    // =========================================================================
    // View Configuration (LayerShell)
    // =========================================================================
    
    /**
     * @brief Configures a window as a dock panel surface.
     * 
     * Sets up LayerShell properties for the window including:
     * - Layer (top/overlay for panels)
     * - Anchor points for edge positioning
     * - Exclusive zone for struts
     * - Keyboard interactivity
     * 
     * @param window The QWindow to configure
     * @param edge The dock edge position
     * @param visibility The visibility mode
     * @return true if configuration succeeded
     */
    bool configurePanelSurface(QWindow *window, NSE::DockEdge edge, 
                               NSE::Visibility visibility);
    
    /**
     * @brief Updates the exclusive zone (strut) for a dock window.
     * 
     * The exclusive zone prevents other windows from overlapping
     * the dock area.
     * 
     * @param window The dock window
     * @param size Size of the exclusive zone in pixels
     */
    void setExclusiveZone(QWindow *window, int size);
    
    /**
     * @brief Clears the exclusive zone, allowing windows to overlap.
     * @param window The dock window
     */
    void clearExclusiveZone(QWindow *window);

    // =========================================================================
    // AbstractWindowInterface Overrides
    // =========================================================================
    
    void setViewExtraFlags(QObject *view, bool isPanelWindow, 
                          NSE::Types::Visibility mode) override;
    void setViewStruts(QWindow &view, const QRect &rect,
                      Plasma::Types::Location location) override;
    void removeViewStruts(QWindow &view) override;
    
    WindowId activeWindow() override;
    void skipTaskBar(const QDialog &dialog) override;
    void slideWindow(QWindow &view, Slide location) override;
    void enableBlurBehind(QWindow &view) override;
    void setActiveEdge(QWindow *view, bool active) override;
    void setFrameExtents(QWindow *view, const QMargins &extents) override;
    void setInputMask(QWindow *window, const QRect &rect) override;
    
    WindowInfoWrap requestInfoActive() override;
    WindowInfoWrap requestInfo(WindowId wid) override;
    AppData appDataFor(WindowId wid) override;
    QIcon iconFor(WindowId wid) override;
    
    WindowId winIdFor(QString appId, QString title) override;
    WindowId winIdFor(QString appId, QRect geometry) override;
    
    bool windowCanBeDragged(WindowId wid) override;
    bool windowCanBeMaximized(WindowId wid) override;
    
    void requestActivate(WindowId wid) override;
    void requestClose(WindowId wid) override;
    void requestMoveWindow(WindowId wid, QPoint from) override;
    void requestToggleIsOnAllDesktops(WindowId wid) override;
    void requestToggleKeepAbove(WindowId wid) override;
    void setKeepAbove(WindowId wid, bool active) override;
    void setKeepBelow(WindowId wid, bool active) override;
    void requestToggleMinimized(WindowId wid) override;
    void requestToggleMaximized(WindowId wid) override;
    
    void setWindowOnActivities(const WindowId &wid, 
                               const QStringList &activities) override;
    void switchToNextVirtualDesktop() override;
    void switchToPreviousVirtualDesktop() override;
    
    void registerIgnoredWindow(WindowId wid) override;
    void unregisterIgnoredWindow(WindowId wid) override;

signals:
    /**
     * @brief Emitted when a compositor error occurs.
     * @param message Human-readable error description
     */
    void compositorError(const QString &message);
    
    /**
     * @brief Emitted when LayerShell becomes available or unavailable.
     * @param available true if LayerShell is now available
     */
    void layerShellAvailabilityChanged(bool available);

private:
    // =========================================================================
    // Internal Helpers
    // =========================================================================
    
    /**
     * @brief Converts NSE::DockEdge to LayerShellQt anchor flags.
     * @param edge The dock edge
     * @return Combination of LayerShellQt::Window::Anchor flags
     */
    [[nodiscard]] LayerShellQt::Window::Anchors edgeToAnchors(NSE::DockEdge edge) const;
    
    /**
     * @brief Converts NSE::Visibility to LayerShellQt layer.
     * @param visibility The visibility mode
     * @return Appropriate layer (Top, Overlay, etc.)
     */
    [[nodiscard]] LayerShellQt::Window::Layer visibilityToLayer(NSE::Visibility visibility) const;
    
    /**
     * @brief Gets or creates a LayerShellQt::Window for a QWindow.
     * @param window The Qt window
     * @return The LayerShell window wrapper, or nullptr on failure
     */
    [[nodiscard]] LayerShellQt::Window* getLayerWindow(QWindow *window);
    
    /**
     * @brief Logs an error and stores it for retrieval.
     * @param context The operation that failed
     * @param message Detailed error message
     */
    void setError(const QString &context, const QString &message);

    // =========================================================================
    // Member Variables
    // =========================================================================
    
    /// Parent Corona object
    QPointer<NSE::Corona> m_corona;
    
    /// Cached LayerShell window instances
    QHash<QWindow*, LayerShellQt::Window*> m_layerWindows;
    
    /// List of ignored windows (not tracked by dock)
    QList<WindowId> m_ignoredWindows;
    
    /// Current virtual desktop ID
    QString m_currentDesktop;
    
    /// List of virtual desktop IDs
    QStringList m_desktops;
    
    /// Initialisation state
    bool m_initialised{false};
    
    /// Last error message for diagnostics
    QString m_lastError;
};

} // namespace WindowSystem
} // namespace NSE

#endif // NSEWAYLANDINTERFACE_H
