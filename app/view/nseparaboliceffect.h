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
 * SPDX-FileCopyrightText: 2020 Michail Vourlakos <mvourlakos@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef NSEPARABOLICEFFECT_H
#define NSEPARABOLICEFFECT_H

#include "../NSETypes.h"

// Qt
#include <QEvent>
#include <QObject>
#include <QQuickItem>
#include <QPointer>
#include <QPointF>
#include <QTimer>
#include <QEasingCurve>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(nseParabolic)

namespace NSE {
class View;
}

namespace NSE {
namespace ViewPart {

/**
 * @brief The NSE Parabolic Zoom Effect for the Syndromatic Desktop Experience.
 * 
 * This class provides the "Fluid Motion" icon magnification effect for SynDock.
 * It handles mouse tracking, item hover detection, and coordinates with QML
 * for the visual zoom animation.
 * 
 * Key Features:
 * - Smooth bezier curve interpolation via QEasingCurve::OutCubic
 * - Configurable zoom factor (default: 40% as per SynOS standards)
 * - Zero-latency mouse tracking integration
 * - QML bridge for rendering (actual animation done in QML/shaders)
 * 
 * The visual interpolation follows a parabolic curve where items closer to
 * the mouse cursor are scaled larger, creating the iconic dock zoom effect.
 * 
 * @note For optimal performance, the actual rendering should use RHI shaders
 *       in the QML layer rather than CPU-bound transformations.
 */
class NSEParabolicEffect : public QObject
{
    Q_OBJECT
    
    /**
     * @brief The currently hovered item receiving the parabolic effect.
     */
    Q_PROPERTY(QQuickItem* currentItem READ currentItem 
               WRITE setCurrentItem NOTIFY currentItemChanged)
    
    /**
     * @brief The zoom factor for maximum magnification (0.0 - 1.0).
     * 
     * A value of 0.40 means 40% larger than base size at the cursor position.
     * Default: NSE::Defaults::HoverZoomFactor (0.40)
     */
    Q_PROPERTY(qreal zoomFactor READ zoomFactor 
               WRITE setZoomFactor NOTIFY zoomFactorChanged)
    
    /**
     * @brief Animation duration in milliseconds for zoom transitions.
     * 
     * Default: NSE::Defaults::AnimationDuration (150ms)
     */
    Q_PROPERTY(int animationDuration READ animationDuration 
               WRITE setAnimationDuration NOTIFY animationDurationChanged)

public:
    /**
     * @brief Constructs the NSE Parabolic Effect.
     * @param parent The parent View object
     */
    explicit NSEParabolicEffect(NSE::View *parent);
    
    /**
     * @brief Destructor.
     */
    ~NSEParabolicEffect() override;
    
    // =========================================================================
    // Property Accessors
    // =========================================================================
    
    /**
     * @brief Gets the currently hovered parabolic item.
     * @return Pointer to the QQuickItem, or nullptr if none
     */
    [[nodiscard]] QQuickItem* currentItem() const { return m_currentItem; }
    
    /**
     * @brief Sets the currently hovered parabolic item.
     * @param item The QML item to apply the effect to
     */
    void setCurrentItem(QQuickItem *item);
    
    /**
     * @brief Gets the current zoom factor.
     * @return Zoom factor (0.0 - 1.0)
     */
    [[nodiscard]] qreal zoomFactor() const { return m_zoomFactor; }
    
    /**
     * @brief Sets the zoom factor.
     * @param factor New zoom factor (clamped to 0.0 - 1.0)
     */
    void setZoomFactor(qreal factor);
    
    /**
     * @brief Gets the animation duration.
     * @return Duration in milliseconds
     */
    [[nodiscard]] int animationDuration() const { return m_animationDuration; }
    
    /**
     * @brief Sets the animation duration.
     * @param duration Duration in milliseconds
     */
    void setAnimationDuration(int duration);
    
    // =========================================================================
    // QML Invocable Methods
    // =========================================================================
    
    /**
     * @brief Calculates the zoom scale for an item based on distance from cursor.
     * 
     * Uses the parabolic curve function to determine the appropriate scale
     * factor for an item given its distance from the mouse cursor.
     * 
     * @param distance Distance from cursor in pixels
     * @param maxDistance Maximum effect distance (typically dock width/2)
     * @return Scale factor (1.0 = no zoom, 1.4 = 40% zoom with default settings)
     */
    Q_INVOKABLE [[nodiscard]] qreal calculateZoomScale(qreal distance, qreal maxDistance) const;
    
    /**
     * @brief Applies easing to a raw zoom value for smooth animation.
     * 
     * Uses QEasingCurve::OutCubic for natural deceleration.
     * 
     * @param rawValue Raw zoom value (0.0 - 1.0)
     * @return Eased value following the bezier curve
     */
    Q_INVOKABLE [[nodiscard]] qreal applyEasing(qreal rawValue) const;

signals:
    void currentItemChanged();
    void zoomFactorChanged();
    void animationDurationChanged();
    
    /**
     * @brief Emitted when a parabolic move event should be sent to QML.
     * @param localX X position relative to the item
     * @param localY Y position relative to the item
     */
    void parabolicMoveRequested(qreal localX, qreal localY);

private slots:
    void onCurrentItemChanged();
    void onViewEvent(QEvent *e);
    void nullifyCurrentItem();

private:
    // =========================================================================
    // Member Variables
    // =========================================================================
    
    /// Parent view reference
    QPointer<NSE::View> m_view;
    
    /// Currently hovered item
    QPointer<QQuickItem> m_currentItem;
    
    /// Last mouse position for orphan move events
    QPointF m_lastPosition;
    
    /// Timer to nullify item when mouse leaves
    QTimer m_nullifier;
    
    /// Easing curve for smooth animations
    QEasingCurve m_easingCurve{QEasingCurve::OutCubic};
    
    /// Zoom factor (default: 40%)
    qreal m_zoomFactor{NSE::Defaults::HoverZoomFactor};
    
    /// Animation duration in ms
    int m_animationDuration{NSE::Defaults::AnimationDuration};
};

} // namespace ViewPart
} // namespace NSE

#endif // NSEPARABOLICEFFECT_H
