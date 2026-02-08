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

#include "nseparaboliceffect.h"
#include "view.h"

// Qt
#include <QMetaObject>
#include <QMouseEvent>
#include <QtMath>

Q_LOGGING_CATEGORY(nseParabolic, "nse.parabolic", QtInfoMsg)

namespace NSE {
namespace ViewPart {

// =============================================================================
// Construction / Destruction
// =============================================================================

NSEParabolicEffect::NSEParabolicEffect(NSE::View *parent)
    : QObject(parent)
    , m_view(parent)
{
    // Configure the nullifier timer for responsive hover tracking
    // Short interval prevents false "not hovered" signals during rapid movement
    m_nullifier.setInterval(1);
    m_nullifier.setSingleShot(true);
    
    connect(&m_nullifier, &QTimer::timeout, 
            this, &NSEParabolicEffect::nullifyCurrentItem);
    
    connect(this, &NSEParabolicEffect::currentItemChanged, 
            this, &NSEParabolicEffect::onCurrentItemChanged);
    
    // Connect to view events for mouse tracking
    if (m_view) {
        connect(m_view, &NSE::View::eventTriggered, 
                this, &NSEParabolicEffect::onViewEvent);
    }
    
    qCInfo(nseParabolic) << "NSE Parabolic Effect initialised"
                         << "| Zoom:" << (m_zoomFactor * 100) << "%"
                         << "| Duration:" << m_animationDuration << "ms";
}

NSEParabolicEffect::~NSEParabolicEffect()
{
    m_nullifier.stop();
}

// =============================================================================
// Property Accessors
// =============================================================================

void NSEParabolicEffect::setCurrentItem(QQuickItem *item)
{
    if (m_currentItem == item) {
        return;
    }
    
    // Notify the previous item that it's no longer hovered
    if (m_currentItem) {
        QMetaObject::invokeMethod(m_currentItem, "parabolicExited", 
                                  Qt::QueuedConnection);
    }
    
    m_currentItem = item;
    emit currentItemChanged();
}

void NSEParabolicEffect::setZoomFactor(qreal factor)
{
    // Clamp to valid range
    factor = qBound(0.0, factor, 1.0);
    
    if (qFuzzyCompare(m_zoomFactor, factor)) {
        return;
    }
    
    m_zoomFactor = factor;
    emit zoomFactorChanged();
    
    qCDebug(nseParabolic) << "Zoom factor changed to:" << (factor * 100) << "%";
}

void NSEParabolicEffect::setAnimationDuration(int duration)
{
    if (m_animationDuration == duration) {
        return;
    }
    
    m_animationDuration = qMax(0, duration);
    emit animationDurationChanged();
}

// =============================================================================
// Calculation Methods
// =============================================================================

qreal NSEParabolicEffect::calculateZoomScale(qreal distance, qreal maxDistance) const
{
    if (maxDistance <= 0 || distance < 0) {
        return 1.0;  // No zoom
    }
    
    // Normalise distance to 0.0 - 1.0 range
    const qreal normalisedDistance = qMin(distance / maxDistance, 1.0);
    
    // Apply parabolic curve: scale = 1 - distance²
    // This gives maximum zoom at distance 0, falling off parabolically
    const qreal parabolicValue = 1.0 - (normalisedDistance * normalisedDistance);
    
    // Apply easing for smoother visual effect
    const qreal easedValue = applyEasing(parabolicValue);
    
    // Calculate final scale: base (1.0) + zoom contribution
    const qreal scale = 1.0 + (easedValue * m_zoomFactor);
    
    return scale;
}

qreal NSEParabolicEffect::applyEasing(qreal rawValue) const
{
    // Clamp input to valid range
    rawValue = qBound(0.0, rawValue, 1.0);
    
    // Apply the easing curve (OutCubic by default)
    return m_easingCurve.valueForProgress(rawValue);
}

// =============================================================================
// Event Handling
// =============================================================================

void NSEParabolicEffect::onViewEvent(QEvent *e)
{
    if (!e) {
        return;
    }
    
    switch (e->type()) {
        case QEvent::Leave:
            // Mouse left the view entirely
            setCurrentItem(nullptr);
            break;
            
        case QEvent::MouseMove: {
            auto *me = dynamic_cast<QMouseEvent*>(e);
            if (!me) break;
            
            if (m_currentItem) {
                // Map mouse position to item's local coordinates
                const QPointF localPos = m_currentItem->mapFromScene(me->position());
                
                if (m_currentItem->contains(localPos)) {
                    // Still within the current item
                    m_nullifier.stop();
                    
                    // Send move event to QML
                    QMetaObject::invokeMethod(m_currentItem,
                                              "parabolicMove",
                                              Qt::QueuedConnection,
                                              Q_ARG(qreal, localPos.x()),
                                              Q_ARG(qreal, localPos.y()));
                    
                    emit parabolicMoveRequested(localPos.x(), localPos.y());
                } else {
                    // Mouse moved outside current item
                    m_lastPosition = me->position();
                    m_nullifier.start();
                }
            } else {
                // No current item, store position for future item entry
                m_lastPosition = me->position();
            }
            break;
        }
        
        default:
            break;
    }
}

void NSEParabolicEffect::onCurrentItemChanged()
{
    m_nullifier.stop();
    
    if (m_currentItem) {
        // Map the last known position to the new item
        const QPointF localPos = m_currentItem->mapFromScene(m_lastPosition);
        
        if (m_currentItem->contains(localPos)) {
            // Send enter event to QML
            QMetaObject::invokeMethod(m_currentItem,
                                      "parabolicEntered",
                                      Qt::QueuedConnection,
                                      Q_ARG(qreal, localPos.x()),
                                      Q_ARG(qreal, localPos.y()));
        }
    }
}

void NSEParabolicEffect::nullifyCurrentItem()
{
    setCurrentItem(nullptr);
}

} // namespace ViewPart
} // namespace NSE
