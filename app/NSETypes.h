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
 */

#ifndef NSETYPES_H
#define NSETYPES_H

#include <QtGlobal>
#include <QEvent>
#include <QString>

/**
 * @brief The NSE namespace contains all SynDock types and constants.
 * 
 * This is the root namespace for the Syndromatic Desktop Experience (SynOS)
 * docking system. All custom classes, enumerations and configuration values
 * are encapsulated within this namespace.
 */
namespace NSE {

/**
 * @brief Default configuration values for the SynOS dock experience.
 * 
 * These constants define the "SynOS Default" layout which is optimised
 * for zero-latency rendering and visual consistency across the desktop.
 */
namespace Defaults {
    /// Base icon size in pixels (64px standard)
    constexpr int IconSize = 64;
    
    /// Maximum icon size when parabolic zoom is at full effect
    constexpr int MaxIconSize = 128;
    
    /// Parabolic hover zoom factor (0.40 = 40% magnification)
    constexpr float HoverZoomFactor = 0.40f;
    
    /// Panel background opacity (0.30 = 30% transparency)
    constexpr float BackgroundOpacity = 0.30f;
    
    /// Animation duration in milliseconds for smooth transitions
    constexpr int AnimationDuration = 150;
    
    /// Minimum spacing between dock items in pixels
    constexpr int ItemSpacing = 4;
}

/**
 * @brief Visibility modes for dock auto-hide behaviour.
 * 
 * Controls how the dock responds to window positioning and user interaction.
 * Note: British English spelling used throughout (behaviour, not behavior).
 */
enum class Visibility {
    /// Dock is always visible, never hides
    AlwaysVisible,
    
    /// Dock hides when not in use, shows on mouse approach
    AutoHide,
    
    /// Dock hides when the active window would overlap it
    DodgeActive,
    
    /// Dock hides when a maximised window is present
    DodgeMaximised,
    
    /// Dock hides when any window would overlap it
    DodgeAllWindows,
    
    /// Windows go below the dock (dock always on top)
    WindowsGoBelow,
    
    /// Windows can cover the dock when focused
    WindowsCanCover,
    
    /// Windows always cover the dock
    WindowsAlwaysCover,
    
    /// Dock acts as a sidebar (vertical orientation)
    SideBar
};

/**
 * @brief Edge positions for dock placement.
 * 
 * Defines where the dock is positioned on the screen.
 * Uses British English spelling (Centre, not Center).
 */
enum class DockEdge {
    Bottom,     ///< Dock at the bottom edge of the screen
    Top,        ///< Dock at the top edge of the screen
    Left,       ///< Dock at the left edge of the screen
    Right,      ///< Dock at the right edge of the screen
    Centre      ///< Floating dock in the centre (future feature)
};

/**
 * @brief Alignment options for dock within its edge.
 */
enum class Alignment {
    Left,       ///< Align dock to the left/top of its edge
    Centre,     ///< Centre dock on its edge (default)
    Right,      ///< Align dock to the right/bottom of its edge
    Justify     ///< Stretch dock to fill its entire edge
};

/**
 * @brief Memory usage modes for layout management.
 */
enum class MemoryUsage {
    /// Single layout active at a time
    SingleLayout,
    
    /// Multiple layouts can be active based on activities
    MultipleLayouts
};

/**
 * @brief Window tracking states for visibility management.
 */
enum class WindowState {
    Normal,
    Maximised,
    FullScreen,
    Minimised
};

/**
 * @brief Converts DockEdge to a human-readable string.
 * @param edge The dock edge to convert
 * @return QString representation of the edge
 */
[[nodiscard]] inline QString dockEdgeToString(DockEdge edge) {
    switch (edge) {
        case DockEdge::Bottom: return QStringLiteral("Bottom");
        case DockEdge::Top:    return QStringLiteral("Top");
        case DockEdge::Left:   return QStringLiteral("Left");
        case DockEdge::Right:  return QStringLiteral("Right");
        case DockEdge::Centre: return QStringLiteral("Centre");
    }
    return QStringLiteral("Unknown");
}

/**
 * @brief Converts Visibility mode to a human-readable string.
 * @param mode The visibility mode to convert
 * @return QString representation of the visibility mode
 */
[[nodiscard]] inline QString visibilityToString(Visibility mode) {
    switch (mode) {
        case Visibility::AlwaysVisible:      return QStringLiteral("Always Visible");
        case Visibility::AutoHide:           return QStringLiteral("Auto Hide");
        case Visibility::DodgeActive:        return QStringLiteral("Dodge Active");
        case Visibility::DodgeMaximised:     return QStringLiteral("Dodge Maximised");
        case Visibility::DodgeAllWindows:    return QStringLiteral("Dodge All Windows");
        case Visibility::WindowsGoBelow:     return QStringLiteral("Windows Go Below");
        case Visibility::WindowsCanCover:    return QStringLiteral("Windows Can Cover");
        case Visibility::WindowsAlwaysCover: return QStringLiteral("Windows Always Cover");
        case Visibility::SideBar:            return QStringLiteral("Sidebar");
    }
    return QStringLiteral("Unknown");
}

} // namespace NSE

#endif // NSETYPES_H
