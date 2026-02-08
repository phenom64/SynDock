/* This file is a part of the Atmo desktop experience's SynDock project for SynOS.
 * Copyright (C) 2026 Syndromatic Ltd. All rights reserved
 * Designed by Kavish Krishnakumar in Manchester.
 *
 * Based on Latte Dock:
 * SPDX-FileCopyrightText: 2016 Smith AR <audoban@openmailbox.org>
 * SPDX-FileCopyrightText: 2016 Michail Vourlakos <mvourlakos@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef NSECORONAINTERFACE_H
#define NSECORONAINTERFACE_H

// local
#include <coretypes.h>
#include "plasma/quick/configview.h"
#include "layouts/storage.h"
#include "view/panelshadows_p.h"

// Qt
#include <QObject>
#include <QTimer>

// Plasma
#include <Plasma/Corona>

// KDE
#include <KAboutApplicationDialog>

namespace KDeclarative {
class QmlObjectSharedEngine;
}

namespace Plasma {
class Corona;
class Containment;
class Types;
}

namespace PlasmaQuick {
class ConfigView;
}

namespace KActivities {
class Consumer;
}

/* NOTE: KWayland::Client is deprecated in KF6/Plasma 6
 * LayerShellQt is now the preferred method for panel surfaces.
 * This forward declaration is retained for compatibility during transition.
 */
namespace KWayland {
namespace Client {
class PlasmaShell;
}
}

/**
 * @namespace NSE
 * @brief The Syndromatic Desktop Experience namespace
 * 
 * All SynDock classes use the NSE (Native Syndromatic Experience) prefix
 * following the SynOS coding conventions.
 */
namespace NSE {

class CentralLayout;
class ScreenPool;
class GlobalShortcuts;
class UniversalSettings;
class View;
class ViewSettingsFactory;

namespace Indicator {
class Factory;
}

namespace Layout {
class GenericLayout;
}

namespace Layouts {
class Manager;
}

namespace PlasmaExtended {
class ScreenGeometries;
class ScreenPool;
class Theme;
}

namespace Templates {
class Manager;
}

namespace WindowSystem {
class AbstractWindowInterface;
}

/**
 * @class Corona
 * @brief The main SynDock corona interface
 * 
 * Corona manages the dock's lifecycle, screen handling, and layout management.
 * This is the primary entry point for the SynDock application.
 * 
 * @note D-Bus interface updated from org.kde.SynDock to org.syndromatic.SynDock
 */
class Corona : public Plasma::Corona
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.syndromatic.SynDock")

public:
    /**
     * @brief Constructs the SynDock Corona
     * @param defaultLayoutOnStartup Whether to load the default layout
     * @param layoutNameOnStartUp Specific layout name to load
     * @param addViewTemplateName Template for adding new views
     * @param userSetMemoryUsage Memory usage setting (-1 for default)
     * @param parent Parent QObject
     */
    Corona(bool defaultLayoutOnStartup = false,
           QString layoutNameOnStartUp = QString(),
           QString addViewTemplateName = QString(),
           int userSetMemoryUsage = -1,
           QObject *parent = nullptr);
    virtual ~Corona();

    bool inQuit() const;

    int numScreens() const override;
    QRect screenGeometry(int id) const override;
    QRegion availableScreenRegion(int id) const override;
    QRect availableScreenRect(int id) const override;

    /**
     * @brief Calculates available screen rectangle with specific criteria
     * @param id Screen ID
     * @param activityid Activity to consider (empty for all)
     * @param ignoreModes Visibility modes to exclude from calculation
     * @param ignoreEdges Screen edges to exclude
     * @param ignoreExternalPanels Whether to ignore external panels
     * @param desktopUse Whether this is for desktop use
     * @return Available screen rectangle
     */
    QRect availableScreenRectWithCriteria(int id,
                                          QString activityid = QString(),
                                          QList<Types::Visibility> ignoreModes = QList<Types::Visibility>(),
                                          QList<Plasma::Types::Location> ignoreEdges = QList<Plasma::Types::Location>(),
                                          bool ignoreExternalPanels = true,
                                          bool desktopUse = false) const;

    QRegion availableScreenRegionWithCriteria(int id,
                                              QString activityid = QString(),
                                              QList<Types::Visibility> ignoreModes = QList<Types::Visibility>(),
                                              QList<Plasma::Types::Location> ignoreEdges = QList<Plasma::Types::Location>(),
                                              bool ignoreExternalPanels = true,
                                              bool desktopUse = false) const;

    int screenForContainment(const Plasma::Containment *containment) const override;

    KWayland::Client::PlasmaShell *waylandCoronaInterface() const;

    KActivities::Consumer *activitiesConsumer() const;
    GlobalShortcuts *globalShortcuts() const;
    ScreenPool *screenPool() const;
    UniversalSettings *universalSettings() const;
    ViewSettingsFactory *viewSettingsFactory() const;
    Layouts::Manager *layoutsManager() const;   
    Templates::Manager *templatesManager() const;

    Indicator::Factory *indicatorFactory() const;

    PlasmaExtended::ScreenPool *plasmaScreenPool() const;
    PlasmaExtended::Theme *themeExtended() const;

    WindowSystem::AbstractWindowInterface *wm() const;

    PanelShadows *dialogShadows() const;

    //! Import and load application after exit operations complete
    void importFullConfiguration(const QString &file);

    //! Context menu functions
    void quitApplication();
    void switchToLayout(QString layout);
    void importLayoutFile(const QString &filepath, const QString &suggestedLayoutName = QString());
    void showSettingsWindow(int page);

    QStringList contextMenuData(const uint &containmentId);
    QStringList viewTemplatesData();

public slots:
    void aboutApplication();
    void activateLauncherMenu();
    void loadDefaultLayout() override;

    void setAutostart(const bool &enabled);

    void addView(const uint &containmentId, const QString &templateId);
    void duplicateView(const uint &containmentId);
    void exportViewTemplate(const uint &containmentId);
    void moveViewToLayout(const uint &containmentId, const QString &layoutName);
    void removeView(const uint &containmentId);

    void setBackgroundFromBroadcast(QString activity, QString screenName, QString filename);
    void setBroadcastedBackgroundsEnabled(QString activity, QString screenName, bool enabled);
    void showAlternativesForApplet(Plasma::Applet *applet);
    void toggleHiddenState(QString layoutName, QString viewName, QString screenName, int screenEdge);

    //! Values separated with a "-" character
    void windowColourScheme(QString windowIdAndScheme);
    void updateDockItemBadge(QString identifier, QString value);

    void unload();

signals:
    void configurationShown(PlasmaQuick::ConfigView *configView);
    void viewLocationChanged();
    void raiseViewsTemporaryChanged();
    void availableScreenRectChangedFrom(NSE::View *origin);
    void availableScreenRegionChangedFrom(NSE::View *origin);
    void verticalUnityViewHasFocus();

private slots:
    void alternativesVisibilityChanged(bool visible);
    void load();

    void onAboutToQuit();

    void onScreenAdded(QScreen *screen);
    void onScreenRemoved(QScreen *screen);
    void onScreenCountChanged();
    void onScreenGeometryChanged(const QRect &geometry);
    void syncDockViewsToScreens();

private:
    void cleanConfig();
    void qmlRegisterTypes() const;
    void setupWaylandIntegration();

    bool appletExists(uint containmentId, uint appletId) const;
    bool containmentExists(uint id) const;

    int primaryScreenId() const;

    QStringList containmentsIds();
    QStringList appletsIds();

    Layout::GenericLayout *layout(QString name) const;
    CentralLayout *centralLayout(QString name) const;

private:
    bool m_activitiesStarting{true};
    bool m_defaultLayoutOnStartup{false};
    bool m_inStartup{true};
    bool m_inQuit{false};
    bool m_quitTimedEnded{false};

    int m_userSetMemoryUsage{-1};

    QString m_layoutNameOnStartUp;
    QString m_startupAddViewTemplateName;
    QString m_importFullConfigurationFile;

    QList<KDeclarative::QmlObjectSharedEngine *> m_alternativesObjects;

    QTimer m_viewsScreenSyncTimer;

    KActivities::Consumer *m_activitiesConsumer;
    QPointer<KAboutApplicationDialog> aboutDialog;

    ScreenPool *m_screenPool{nullptr};
    UniversalSettings *m_universalSettings{nullptr};
    ViewSettingsFactory *m_viewSettingsFactory{nullptr};
    GlobalShortcuts *m_globalShortcuts{nullptr};

    Indicator::Factory *m_indicatorFactory{nullptr};
    Layouts::Manager *m_layoutsManager{nullptr};
    Templates::Manager *m_templatesManager{nullptr};

    PlasmaExtended::ScreenGeometries *m_plasmaGeometries{nullptr};
    PlasmaExtended::ScreenPool *m_plasmaScreenPool{nullptr};
    PlasmaExtended::Theme *m_themeExtended{nullptr};

    WindowSystem::AbstractWindowInterface *m_wm{nullptr};

    PanelShadows *m_dialogShadows{nullptr};

    KWayland::Client::PlasmaShell *m_waylandCorona{nullptr};

    friend class GlobalShortcuts;
    friend class Layouts::Manager;
    friend class Layouts::Storage;
};

} // namespace NSE

// Compatibility alias during transition
namespace NSE = NSE;

#endif // NSECORONAINTERFACE_H
