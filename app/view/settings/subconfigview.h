/*
    SPDX-FileCopyrightText: 2020 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SUBCONFIGVIEW_H
#define SUBCONFIGVIEW_H

// local
#include <coretypes.h>
#include "../../wm/windowinfowrap.h"

//Qt
#include <QObject>
#include <QPointer>
#include <QQuickView>
#include <QTimer>

// Plasma
#include <Plasma/FrameSvg>

namespace KWayland {
namespace Client {
class PlasmaShellSurface;
}
}

namespace NSE {
class Corona;
class View;
}


namespace NSE {
namespace ViewPart {

class SubConfigView : public QQuickView
{
    Q_OBJECT
    Q_PROPERTY(Plasma::FrameSvg::EnabledBorders enabledBorders READ enabledBorders NOTIFY enabledBordersChanged)

public:
    SubConfigView(NSE::View *view, const QString &title, const bool &isNormalWindow = true);
    ~SubConfigView() override;

    virtual void requestActivate();

    QString validTitle() const;

    Plasma::FrameSvg::EnabledBorders enabledBorders() const;

    NSE::Corona *corona() const;
    NSE::View *parentView() const;
    virtual void setParentView(NSE::View *view, const bool &immediate = false);
    virtual void showAfter(int msecs = 0);

    NSE::WindowSystem::WindowId trackedWindowId();
    KWayland::Client::PlasmaShellSurface *surface();

public slots:
    virtual void syncGeometry() = 0;

signals:
    void enabledBordersChanged();

protected:
    virtual void syncSlideEffect();

    virtual void init();
    virtual void initParentView(NSE::View *view);
    virtual void updateEnabledBorders() = 0;

    void showEvent(QShowEvent *ev) override;
    bool event(QEvent *e) override;

    virtual Qt::WindowFlags wFlags() const;

protected:
    bool m_isNormalWindow{true};
    QTimer m_screenSyncTimer;

    QPointer<NSE::View> m_latteView;

    QList<QMetaObject::Connection> connections;
    QList<QMetaObject::Connection> viewconnections;

    Plasma::FrameSvg::EnabledBorders m_enabledBorders{Plasma::FrameSvg::AllBorders};

    NSE::Corona *m_corona{nullptr};
    KWayland::Client::PlasmaShellSurface *m_shellSurface{nullptr};

private slots:
    void updateWaylandId();

private:
    void setupWaylandIntegration();

private:
    QString m_validTitle;

    QTimer m_showTimer;

    NSE::WindowSystem::WindowId m_waylandWindowId;
};

}
}
#endif //SUBCONFIGVIEW_H

