/*
    SPDX-FileCopyrightText: 2019 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef WINDOWSYSTEMWINDOWSTRACKER_H
#define WINDOWSYSTEMWINDOWSTRACKER_H

// local
#include <coretypes.h>
#include "../windowinfowrap.h"

// Qt
#include <QObject>

#include <QHash>
#include <QMap>
#include <QTimer>


namespace NSE {
class View;
namespace Layout {
class GenericLayout;
}
namespace WindowSystem {
class AbstractWindowInterface;
class SchemeColors;
namespace Tracker {
class LastActiveWindow;
class TrackedLayoutInfo;
class TrackedViewInfo;
}
}
}

namespace NSE {
namespace WindowSystem {
namespace Tracker {

class Windows : public QObject {
    Q_OBJECT

public:
    Windows(AbstractWindowInterface *parent);
    ~Windows() override;

    void addView(NSE::View *view);
    void removeView(NSE::View *view);

    //! Views Tracking (current screen specific)
    bool enabled(NSE::View *view);
    void setEnabled(NSE::View *view, const bool enabled);

    bool activeWindowMaximized(NSE::View *view) const;
    bool activeWindowTouching(NSE::View *view) const;
    bool activeWindowTouchingEdge(NSE::View *view) const;
    bool existsWindowActive(NSE::View *view) const;
    bool existsWindowMaximized(NSE::View *view) const;
    bool existsWindowTouching(NSE::View *view) const;
    bool existsWindowTouchingEdge(NSE::View *view) const;
    bool isTouchingBusyVerticalView(NSE::View *view) const;
    SchemeColors *activeWindowScheme(NSE::View *view) const;
    SchemeColors *touchingWindowScheme(NSE::View *view) const;
    LastActiveWindow *lastActiveWindow(NSE::View *view);

    //! Layouts Tracking (all screens)
    bool enabled(NSE::Layout::GenericLayout *layout);
    bool activeWindowMaximized(NSE::Layout::GenericLayout *layout) const;
    bool existsWindowActive(NSE::Layout::GenericLayout *layout) const;
    bool existsWindowMaximized(NSE::Layout::GenericLayout *layout) const;
    SchemeColors *activeWindowScheme(NSE::Layout::GenericLayout *layout) const;
    LastActiveWindow *lastActiveWindow(NSE::Layout::GenericLayout *layout);

    //! Windows management
    bool isValidFor(const WindowId &wid) const;
    QIcon iconFor(const WindowId &wid);
    QString appNameFor(const WindowId &wid);
    WindowInfoWrap infoFor(const WindowId &wid) const;

    AbstractWindowInterface *wm();

signals:
    //! Views
    void enabledChanged(const NSE::View *view);
    void activeWindowMaximizedChanged(const NSE::View *view);
    void activeWindowTouchingChanged(const NSE::View *view);
    void activeWindowTouchingEdgeChanged(const NSE::View *view);
    void existsWindowActiveChanged(const NSE::View *view);
    void existsWindowMaximizedChanged(const NSE::View *view);
    void existsWindowTouchingChanged(const NSE::View *view);
    void existsWindowTouchingEdgeChanged(const NSE::View *view);
    void isTouchingBusyVerticalViewChanged(const NSE::View *view);
    void activeWindowSchemeChanged(const NSE::View *view);
    void touchingWindowSchemeChanged(const NSE::View *view);
    void informationAnnounced(const NSE::View *view);

    //! Layouts
    void enabledChangedForLayout(const NSE::Layout::GenericLayout *layout);
    void activeWindowMaximizedChangedForLayout(const NSE::Layout::GenericLayout *layout);
    void existsWindowActiveChangedForLayout(const NSE::Layout::GenericLayout *layout);
    void existsWindowMaximizedChangedForLayout(const NSE::Layout::GenericLayout *layout);
    void activeWindowSchemeChangedForLayout(const NSE::Layout::GenericLayout *layout);
    void informationAnnouncedForLayout(const NSE::Layout::GenericLayout *layout);

    //! overloading WM signals in order to update first m_windows and afterwards
    //! inform consumers for window changes
    void activeWindowChanged(const WindowId &wid);
    void windowChanged(const WindowId &wid);
    void windowRemoved(const WindowId &wid);

    void applicationDataChanged(const WindowId &wid);

private slots:
    void updateScreenGeometries();

    void addRelevantLayout(NSE::View *view);

    void updateApplicationData();
    void updateRelevantLayouts();
    void updateExtraViewHints();

private:
    void init();
    void initLayoutHints(NSE::Layout::GenericLayout *layout);
    void initViewHints(NSE::View *view);
    void cleanupFaultyWindows();

    void updateAllHints();
    void updateAllHintsAfterTimer();

    //! Views
    void updateHints(NSE::View *view);
    void updateHints(NSE::Layout::GenericLayout *layout);

    void setActiveWindowMaximized(NSE::View *view, bool activeMaximized);
    void setActiveWindowTouching(NSE::View *view, bool activeTouching);
    void setActiveWindowTouchingEdge(NSE::View *view, bool activeTouchingEdge);
    void setExistsWindowActive(NSE::View *view, bool windowActive);
    void setExistsWindowMaximized(NSE::View *view, bool windowMaximized);
    void setExistsWindowTouching(NSE::View *view, bool windowTouching);
    void setExistsWindowTouchingEdge(NSE::View *view, bool windowTouchingEdge);
    void setIsTouchingBusyVerticalView(NSE::View *view, bool viewTouching);
    void setActiveWindowScheme(NSE::View *view, WindowSystem::SchemeColors *scheme);
    void setTouchingWindowScheme(NSE::View *view, WindowSystem::SchemeColors *scheme);

    //! Layouts
    void setActiveWindowMaximized(NSE::Layout::GenericLayout *layout, bool activeMaximized);
    void setExistsWindowActive(NSE::Layout::GenericLayout *layout, bool windowActive);
    void setExistsWindowMaximized(NSE::Layout::GenericLayout *layout, bool windowMaximized);
    void setActiveWindowScheme(NSE::Layout::GenericLayout *layout, WindowSystem::SchemeColors *scheme);

    //! Windows
    bool intersects(NSE::View *view, const WindowInfoWrap &winfo);
    bool isActive(const WindowInfoWrap &winfo);
    bool isActiveInViewScreen(NSE::View *view, const WindowInfoWrap &winfo);
    bool isMaximizedInViewScreen(NSE::View *view, const WindowInfoWrap &winfo);
    bool isTouchingView(NSE::View *view, const WindowSystem::WindowInfoWrap &winfo);
    bool isTouchingViewEdge(NSE::View *view, const WindowInfoWrap &winfo);
    bool isTouchingViewEdge(NSE::View *view, const QRect &windowgeometry);

private:
    //! a timer in order to not overload the views extra hints checking because it is not
    //! really needed that often
    QTimer m_extraViewHintsTimer;

    AbstractWindowInterface *m_wm;
    QHash<NSE::View *, TrackedViewInfo *> m_views;
    QHash<NSE::Layout::GenericLayout *, TrackedLayoutInfo *> m_layouts;

    //! Accept only ALWAYSVISIBLE visibility mode
    QList<NSE::Types::Visibility> m_ignoreModes{
        NSE::Types::AutoHide,
        NSE::Types::DodgeActive,
        NSE::Types::DodgeMaximized,
        NSE::Types::DodgeAllWindows,
        NSE::Types::WindowsGoBelow,
        NSE::Types::WindowsCanCover,
        NSE::Types::WindowsAlwaysCover,
        NSE::Types::SidebarOnDemand,
        NSE::Types::SidebarAutoHide
    };

    QMap<WindowId, WindowInfoWrap> m_windows;

    QTimer m_updateAllHintsTimer;
    //! Some applications delay their application name/icon identification
    //! such as Libreoffice that updates its StartupWMClass after
    //! its startup
    QTimer m_updateApplicationDataTimer;
    QList<WindowId> m_delayedApplicationData;
    QList<WindowId> m_initializedApplicationData;
};

}
}
}

#endif
