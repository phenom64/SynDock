/*
    SPDX-FileCopyrightText: 2018 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "menu.h"

// local
#include "contextmenudata.h"
#include "layoutmenuitemwidget.h"

// Qt
#include <QAction>
#include <QDebug>
#include <QFont>
#include <QMenu>
#include <QtDBus>
#include <QTimer>
#include <QLatin1String>

// KDE
#include <KActionCollection>
#include <KLocalizedString>

// Plasma
#include <Plasma/Containment>
#include <Plasma/Corona>
#include <Plasma/ServiceJob>

const int MEMORYINDEX = 0;
const int ACTIVELAYOUTSINDEX = 1;
const int CURRENTLAYOUTSINDEX = 2;
const int ACTIONSALWAYSSHOWN = 3;
const int LAYOUTMENUINDEX = 4;
const int VIEWLAYOUTINDEX = 5;
const int VIEWTYPEINDEX = 6;

namespace {
constexpr auto SYNDOCK_DBUS_SERVICE = "org.syndromatic.syndock";
constexpr auto SYNDOCK_DBUS_PATH = "/SynDock";
constexpr auto SYNDOCK_DBUS_INTERFACE = "org.syndromatic.SynDock";
constexpr auto SHOW_SETTINGS_ACTION = "_show_syndock_settings_dialog_";

QDBusInterface syndockInterface()
{
    return QDBusInterface(QLatin1String(SYNDOCK_DBUS_SERVICE),
                          QLatin1String(SYNDOCK_DBUS_PATH),
                          QLatin1String(SYNDOCK_DBUS_INTERFACE),
                          QDBusConnection::sessionBus());
}
}

enum LayoutsMemoryUsage
{
    SingleLayout = 0,
    MultipleLayouts
};

enum SynDockConfigPage
{
    LayoutPage = 0,
    PreferencesPage
};

Menu::Menu(QObject *parent, const QVariantList &args)
    : Plasma::ContainmentActions(parent, args)
{
}

Menu::~Menu()
{
    //! sub-menus
    if (m_addViewMenu) {
        m_addViewMenu->deleteLater();
    }

    if (m_switchLayoutsMenu) {
        m_switchLayoutsMenu->deleteLater();
    }

    if (m_moveToLayoutMenu) {
        m_moveToLayoutMenu->deleteLater();
    }

    //! clear menu actions that have been created from submenus
    m_actions.remove(NSE::Data::ContextMenu::ADDVIEWACTION);
    m_actions.remove(NSE::Data::ContextMenu::LAYOUTSACTION);

    //! actions
    qDeleteAll(m_actions.values());
    m_actions.clear();
}

void Menu::restore(const KConfigGroup &config)
{
    Q_UNUSED(config);

    if (!m_actions.isEmpty()) {
        return;
    }

    m_actions[NSE::Data::ContextMenu::SECTIONACTION] = new QAction(this);
    m_actions[NSE::Data::ContextMenu::SECTIONACTION]->setSeparator(true);
    m_actions[NSE::Data::ContextMenu::SECTIONACTION]->setText("SynDock");

    m_actions[NSE::Data::ContextMenu::SEPARATOR1ACTION] = new QAction(this);
    m_actions[NSE::Data::ContextMenu::SEPARATOR1ACTION]->setSeparator(true);

    //! Print Message...
    m_actions[NSE::Data::ContextMenu::PRINTACTION] = new QAction(QIcon::fromTheme("edit"), "Print Message...", this);
    connect(m_actions[NSE::Data::ContextMenu::PRINTACTION], &QAction::triggered, [ = ]() {
        qDebug() << "Action Triggered !!!";
    });

    //! Add Widgets...
    m_actions[NSE::Data::ContextMenu::ADDWIDGETSACTION] = new QAction(QIcon::fromTheme("list-add"), i18n("&Add Widgets..."), this);
    m_actions[NSE::Data::ContextMenu::ADDWIDGETSACTION]->setStatusTip(i18n("Show Widget Explorer"));
    connect(m_actions[NSE::Data::ContextMenu::ADDWIDGETSACTION], &QAction::triggered, this, &Menu::requestWidgetExplorer);
    this->containment()->actions()->addAction(NSE::Data::ContextMenu::ADDWIDGETSACTION, m_actions[NSE::Data::ContextMenu::ADDWIDGETSACTION]);

    /*connect(m_addWidgetsAction, &QAction::triggered, [ = ]() {
        QDBusInterface iface("org.kde.plasmashell", "/PlasmaShell", "", QDBusConnection::sessionBus());

        if (iface.isValid()) {
            iface.call("toggleWidgetExplorer");
        }
    });*/

    //! Edit Dock/Panel...
    m_actions[NSE::Data::ContextMenu::EDITVIEWACTION] = new QAction(QIcon::fromTheme("document-edit"), "Edit Dock...", this);
    connect(m_actions[NSE::Data::ContextMenu::EDITVIEWACTION], &QAction::triggered, this, &Menu::requestConfiguration);
    this->containment()->actions()->addAction(NSE::Data::ContextMenu::EDITVIEWACTION, m_actions[NSE::Data::ContextMenu::EDITVIEWACTION]);


    //! Quit Application
    m_actions[NSE::Data::ContextMenu::QUITSYNDOCKACTION] = new QAction(QIcon::fromTheme("application-exit"), i18nc("quit application", "Quit &SynDock"));
    connect(m_actions[NSE::Data::ContextMenu::QUITSYNDOCKACTION], &QAction::triggered, this, &Menu::quitApplication);
    this->containment()->actions()->addAction(NSE::Data::ContextMenu::QUITSYNDOCKACTION, m_actions[NSE::Data::ContextMenu::QUITSYNDOCKACTION]);

    //! Layouts submenu
    m_switchLayoutsMenu = new QMenu;
    m_actions[NSE::Data::ContextMenu::LAYOUTSACTION] = m_switchLayoutsMenu->menuAction();
    m_actions[NSE::Data::ContextMenu::LAYOUTSACTION]->setText(i18n("&Layouts"));
    m_actions[NSE::Data::ContextMenu::LAYOUTSACTION]->setIcon(QIcon::fromTheme("user-identity"));
    m_actions[NSE::Data::ContextMenu::LAYOUTSACTION]->setStatusTip(i18n("Switch to another layout"));
    this->containment()->actions()->addAction(NSE::Data::ContextMenu::LAYOUTSACTION, m_actions[NSE::Data::ContextMenu::LAYOUTSACTION]);

    connect(m_switchLayoutsMenu, &QMenu::aboutToShow, this, &Menu::populateLayouts);
    connect(m_switchLayoutsMenu, &QMenu::triggered, this, &Menu::switchToLayout);

    //! Add View submenu
    m_addViewMenu = new QMenu;
    m_actions[NSE::Data::ContextMenu::ADDVIEWACTION] = m_addViewMenu->menuAction();
    m_actions[NSE::Data::ContextMenu::ADDVIEWACTION]->setText(i18n("&Add Dock/Panel"));
    m_actions[NSE::Data::ContextMenu::ADDVIEWACTION]->setIcon(QIcon::fromTheme("list-add"));
    m_actions[NSE::Data::ContextMenu::ADDVIEWACTION]->setStatusTip(i18n("Add dock or panel based on specific template"));
    this->containment()->actions()->addAction(NSE::Data::ContextMenu::ADDVIEWACTION, m_actions[NSE::Data::ContextMenu::ADDVIEWACTION]);

    connect(m_addViewMenu, &QMenu::aboutToShow, this, &Menu::populateViewTemplates);
    connect(m_addViewMenu, &QMenu::triggered, this, &Menu::addView);

    //! Move submenu
    m_moveToLayoutMenu = new QMenu;
    m_actions[NSE::Data::ContextMenu::MOVEVIEWACTION] = m_moveToLayoutMenu->menuAction();
    m_actions[NSE::Data::ContextMenu::MOVEVIEWACTION]->setText("Move To Layout");
    m_actions[NSE::Data::ContextMenu::MOVEVIEWACTION]->setIcon(QIcon::fromTheme("transform-move-horizontal"));
    m_actions[NSE::Data::ContextMenu::MOVEVIEWACTION]->setStatusTip(i18n("Move dock or panel to different layout"));
    this->containment()->actions()->addAction(NSE::Data::ContextMenu::MOVEVIEWACTION, m_actions[NSE::Data::ContextMenu::MOVEVIEWACTION]);

    connect(m_moveToLayoutMenu, &QMenu::aboutToShow, this, &Menu::populateMoveToLayouts);
    connect(m_moveToLayoutMenu, &QMenu::triggered, this, &Menu::moveToLayout);

    //! Configure SynDock
    m_actions[NSE::Data::ContextMenu::PREFERENCESACTION] = new QAction(QIcon::fromTheme("configure"), i18nc("global settings window", "&Configure SynDock..."), this);
    this->containment()->actions()->addAction(NSE::Data::ContextMenu::PREFERENCESACTION, m_actions[NSE::Data::ContextMenu::PREFERENCESACTION]);
    connect(m_actions[NSE::Data::ContextMenu::PREFERENCESACTION], &QAction::triggered, [=](){
        QDBusInterface iface = syndockInterface();

        if (iface.isValid()) {
            iface.call("showSettingsWindow", (int)PreferencesPage);
        }
    });

    //! Duplicate Action
    m_actions[NSE::Data::ContextMenu::DUPLICATEVIEWACTION] = new QAction(QIcon::fromTheme("edit-copy"), "Duplicate Dock as Template", this);
    connect(m_actions[NSE::Data::ContextMenu::DUPLICATEVIEWACTION], &QAction::triggered, [=](){
        QDBusInterface iface = syndockInterface();

        if (iface.isValid()) {
            iface.call("duplicateView", containment()->id());
        }
    });
    this->containment()->actions()->addAction(NSE::Data::ContextMenu::DUPLICATEVIEWACTION, m_actions[NSE::Data::ContextMenu::DUPLICATEVIEWACTION]);

    //! Export View Template Action
    m_actions[NSE::Data::ContextMenu::EXPORTVIEWTEMPLATEACTION] = new QAction(QIcon::fromTheme("document-export"), "Export as Template...", this);
    connect(m_actions[NSE::Data::ContextMenu::EXPORTVIEWTEMPLATEACTION], &QAction::triggered, [=](){
        QDBusInterface iface = syndockInterface();

        if (iface.isValid()) {
            iface.call("exportViewTemplate", containment()->id());
        }
    });
    this->containment()->actions()->addAction(NSE::Data::ContextMenu::EXPORTVIEWTEMPLATEACTION, m_actions[NSE::Data::ContextMenu::EXPORTVIEWTEMPLATEACTION]);

    //! Remove Action
    m_actions[NSE::Data::ContextMenu::REMOVEVIEWACTION] = new QAction(QIcon::fromTheme("delete"), "Remove Dock", this);
    connect(m_actions[NSE::Data::ContextMenu::REMOVEVIEWACTION], &QAction::triggered, [=](){
        QDBusInterface iface = syndockInterface();

        if (iface.isValid()) {
            iface.call("removeView", containment()->id());
        }
    });
    this->containment()->actions()->addAction(NSE::Data::ContextMenu::REMOVEVIEWACTION, m_actions[NSE::Data::ContextMenu::REMOVEVIEWACTION]);

    //! Signals
    connect(this->containment(), &Plasma::Containment::userConfiguringChanged, [=](){
        updateVisibleActions();
    });
}

void Menu::requestConfiguration()
{
    if (this->containment()) {
        emit this->containment()->configureRequested(containment());
    }
}

void Menu::requestWidgetExplorer()
{
    if (this->containment()) {
        emit this->containment()->showAddWidgetsInterface(QPointF());
    }
}

QList<QAction *> Menu::contextualActions()
{
    QList<QAction *> actions;

    actions << m_actions[NSE::Data::ContextMenu::SECTIONACTION];
    actions << m_actions[NSE::Data::ContextMenu::PRINTACTION];
    for(int i=0; i<NSE::Data::ContextMenu::ACTIONSEDITORDER.count(); ++i) {
        actions << m_actions[NSE::Data::ContextMenu::ACTIONSEDITORDER[i]];
    }
    actions << m_actions[NSE::Data::ContextMenu::EDITVIEWACTION];

    m_data.clear();
    m_viewTemplates.clear();
    QDBusInterface iface = syndockInterface();

    if (iface.isValid()) {
        QDBusReply<QStringList> contextData = iface.call("contextMenuData", containment()->id());
        m_data = contextData.value();

        QDBusReply<QStringList> templatesData = iface.call("viewTemplatesData");
        m_viewTemplates = templatesData.value();
    }

    if (m_data.size() <= VIEWTYPEINDEX) {
        qWarning() << "Invalid context menu data from org.syndromatic.syndock; using safe defaults.";
        m_data = {
            QString::number(SingleLayout),
            QString(),
            QString(),
            NSE::Data::ContextMenu::ACTIONSALWAYSVISIBLE.join(QStringLiteral(";;")),
            QString(),
            QString(),
            QStringLiteral("0;;0;;0")
        };
    }

    m_actionsAlwaysShown = m_data[ACTIONSALWAYSSHOWN].split(";;");

    updateViewData();

    QString configureActionText = (m_view.type == DockView) ? i18n("&Edit Dock...") : i18n("&Edit Panel...");
    if (m_view.isCloned) {
        configureActionText = (m_view.type == DockView) ? i18n("&Edit Original Dock...") : i18n("&Edit Original Panel...");
    }
    m_actions[NSE::Data::ContextMenu::EDITVIEWACTION]->setText(configureActionText);

    const QString duplicateActionText = (m_view.type == DockView) ? i18n("&Duplicate Dock") : i18n("&Duplicate Panel");
    m_actions[NSE::Data::ContextMenu::DUPLICATEVIEWACTION]->setText(duplicateActionText);

    const QString exportTemplateText = (m_view.type == DockView) ? i18n("E&xport Dock as Template") : i18n("E&xport Panel as Template");
    m_actions[NSE::Data::ContextMenu::EXPORTVIEWTEMPLATEACTION]->setText(exportTemplateText);

    m_activeLayoutNames = m_data[ACTIVELAYOUTSINDEX].split(";;");
    const QString moveText = (m_view.type == DockView) ? i18n("&Move Dock To Layout") : i18n("&Move Panel To Layout");
    m_actions[NSE::Data::ContextMenu::MOVEVIEWACTION]->setText(moveText);

    const QString removeActionText = (m_view.type == DockView) ? i18n("&Remove Dock") : i18n("&Remove Panel");
    m_actions[NSE::Data::ContextMenu::REMOVEVIEWACTION]->setText(removeActionText);

    updateVisibleActions();

    return actions;
}

QAction *Menu::action(const QString &name)
{
    if (m_actions.contains(name)) {
        return m_actions[name];
    }

    return nullptr;
}

void Menu::updateVisibleActions()
{
    if (!m_actions.contains(NSE::Data::ContextMenu::EDITVIEWACTION)
            || !m_actions.contains(NSE::Data::ContextMenu::REMOVEVIEWACTION)) {
        return;
    }

    bool configuring = containment()->isUserConfiguring();

    // normal actions that the user can specify their visibility
    for(auto actionName: m_actions.keys()) {
        if (NSE::Data::ContextMenu::ACTIONSSPECIAL.contains(actionName)) {
            continue;
        } else if (NSE::Data::ContextMenu::ACTIONSALWAYSHIDDEN.contains(actionName)) {
            m_actions[actionName]->setVisible(false);
            continue;
        }

        bool isvisible = m_actionsAlwaysShown.contains(actionName) || configuring;
        m_actions[actionName]->setVisible(isvisible);
    }

    // normal actions with more criteria
    bool isshown = (m_actions[NSE::Data::ContextMenu::MOVEVIEWACTION]->isVisible() && m_activeLayoutNames.count()>1);
    m_actions[NSE::Data::ContextMenu::MOVEVIEWACTION]->setVisible(isshown);

    // special actions
    m_actions[NSE::Data::ContextMenu::EDITVIEWACTION]->setVisible(!configuring);
    m_actions[NSE::Data::ContextMenu::SECTIONACTION]->setVisible(true);

    if (m_view.isCloned) {
        m_actions[NSE::Data::ContextMenu::DUPLICATEVIEWACTION]->setVisible(false);
        m_actions[NSE::Data::ContextMenu::EXPORTVIEWTEMPLATEACTION]->setVisible(false);
        m_actions[NSE::Data::ContextMenu::MOVEVIEWACTION]->setVisible(false);
        m_actions[NSE::Data::ContextMenu::REMOVEVIEWACTION]->setVisible(false);
    }

    // because sometimes they are disabled unexpectedly, we should reenable them
    for(auto actionName: m_actions.keys()) {
        m_actions[actionName]->setEnabled(true);
    }
}


void Menu::populateLayouts()
{
    m_switchLayoutsMenu->clear();

    LayoutsMemoryUsage memoryUsage = static_cast<LayoutsMemoryUsage>((m_data[MEMORYINDEX]).toInt());
    QStringList activeNames = m_data[ACTIVELAYOUTSINDEX].split(";;");
    QStringList currentNames = m_data[CURRENTLAYOUTSINDEX].split(";;");

    QList<LayoutInfo> layoutsmenulist;

    QStringList layoutsdata = m_data[LAYOUTMENUINDEX].split(";;");

    for (int i=0; i<layoutsdata.count(); ++i) {
        QStringList cdata = layoutsdata[i].split("**");
        if (cdata.count() < 3) {
            continue;
        }

        LayoutInfo info;
        info.layoutName = cdata[0];
        info.isBackgroundFileIcon = cdata[1].toInt();
        info.iconName = cdata[2];

        layoutsmenulist << info;
    }

    for (int i = 0; i < layoutsmenulist.count(); ++i) {
        bool isActive = activeNames.contains(layoutsmenulist[i].layoutName);

        bool isCurrent = ((memoryUsage == SingleLayout && isActive)
                          || (memoryUsage == MultipleLayouts && currentNames.contains(layoutsmenulist[i].layoutName)));


        QWidgetAction *action = new QWidgetAction(m_switchLayoutsMenu);
        action->setText(layoutsmenulist[i].layoutName);
        action->setCheckable(true);
        action->setChecked(isCurrent);
        action->setData(layoutsmenulist[i].layoutName);

        LayoutMenuItemWidget *menuitem = new LayoutMenuItemWidget(action, m_switchLayoutsMenu);
        menuitem->setIcon(layoutsmenulist[i].isBackgroundFileIcon, layoutsmenulist[i].iconName);
        action->setDefaultWidget(menuitem);
        m_switchLayoutsMenu->addAction(action);
    }

    m_switchLayoutsMenu->addSeparator();

    QWidgetAction *editaction = new QWidgetAction(m_switchLayoutsMenu);
    editaction->setText(i18n("Edit &Layouts..."));
    editaction->setCheckable(false);
    editaction->setData(QStringLiteral(SHOW_SETTINGS_ACTION));
    editaction->setVisible(false);

    LayoutMenuItemWidget *editmenuitem = new LayoutMenuItemWidget(editaction, m_switchLayoutsMenu);
    editmenuitem->setIcon(false, "document-edit");
    editaction->setDefaultWidget(editmenuitem);
    m_switchLayoutsMenu->addAction(editaction);
}

void Menu::populateMoveToLayouts()
{
    m_moveToLayoutMenu->clear();

    LayoutsMemoryUsage memoryUsage = static_cast<LayoutsMemoryUsage>((m_data[MEMORYINDEX]).toInt());

    if (memoryUsage == LayoutsMemoryUsage::MultipleLayouts) {
        QStringList activeNames = m_data[ACTIVELAYOUTSINDEX].split(";;");
        QStringList currentNames = m_data[CURRENTLAYOUTSINDEX].split(";;");
        QString viewLayoutName = m_data[VIEWLAYOUTINDEX];

        QList<LayoutInfo> layoutsmenulist;

        QStringList layoutsdata = m_data[LAYOUTMENUINDEX].split(";;");

        for (int i=0; i<layoutsdata.count(); ++i) {
            QStringList cdata = layoutsdata[i].split("**");
            if (cdata.count() < 3) {
                continue;
            }

            LayoutInfo info;
            info.layoutName = cdata[0];
            info.isBackgroundFileIcon = cdata[1].toInt();
            info.iconName = cdata[2];

            layoutsmenulist << info;
        }

        for (int i = 0; i < layoutsmenulist.count(); ++i) {
            bool isCurrent = currentNames.contains(layoutsmenulist[i].layoutName) && activeNames.contains(layoutsmenulist[i].layoutName);
            bool isViewCurrentLayout = layoutsmenulist[i].layoutName == viewLayoutName;

            QWidgetAction *action = new QWidgetAction(m_moveToLayoutMenu);
            action->setText(layoutsmenulist[i].layoutName);
            action->setCheckable(true);
            action->setChecked(isViewCurrentLayout);
            action->setData(isViewCurrentLayout ? QString() : layoutsmenulist[i].layoutName);

            LayoutMenuItemWidget *menuitem = new LayoutMenuItemWidget(action, m_moveToLayoutMenu);
            menuitem->setIcon(layoutsmenulist[i].isBackgroundFileIcon, layoutsmenulist[i].iconName);
            action->setDefaultWidget(menuitem);
            m_moveToLayoutMenu->addAction(action);
        }
    }
}

void Menu::updateViewData()
{
    QStringList vdata = m_data[VIEWTYPEINDEX].split(";;");
    if (vdata.count() < 3) {
        m_view = ViewTypeData{ViewType::DockView, false, 0};
        return;
    }

    m_view.type = static_cast<ViewType>((vdata[0]).toInt());
    m_view.isCloned = vdata[1].toInt();
    m_view.clonesCount = vdata[2].toInt();
}

void Menu::populateViewTemplates()
{
    m_addViewMenu->clear();

    for(int i=0; i<m_viewTemplates.count(); ++i) {
        if (i % 2 == 1) {
            //! even records are the templates ids and they have already been registered
            continue;
        }
        if (i + 1 >= m_viewTemplates.count()) {
            break;
        }

        QAction *templateAction = m_addViewMenu->addAction(m_viewTemplates[i]);
        templateAction->setIcon(QIcon::fromTheme("list-add"));
        templateAction->setData(m_viewTemplates[i+1]);
    }

    m_addViewMenu->addSeparator();
    QAction *duplicateAction = m_addViewMenu->addAction(m_actions[NSE::Data::ContextMenu::DUPLICATEVIEWACTION]->text());
    duplicateAction->setToolTip(m_actions[NSE::Data::ContextMenu::DUPLICATEVIEWACTION]->toolTip());
    duplicateAction->setIcon(m_actions[NSE::Data::ContextMenu::DUPLICATEVIEWACTION]->icon());
    connect(duplicateAction, &QAction::triggered, m_actions[NSE::Data::ContextMenu::DUPLICATEVIEWACTION], &QAction::triggered);
}

void Menu::addView(QAction *action)
{
    const QString templateId = action->data().toString();

    QTimer::singleShot(400, [this, templateId]() {
        QDBusInterface iface = syndockInterface();

        if (iface.isValid()) {
            iface.call("addView", containment()->id(), templateId);
        }
    });
}

void Menu::moveToLayout(QAction *action)
{
    const QString layoutName = action->data().toString();

    QTimer::singleShot(400, [this, layoutName]() {
        QDBusInterface iface = syndockInterface();

        if (iface.isValid()) {
            iface.call("moveViewToLayout", containment()->id(), layoutName);
        }
    });
}

void Menu::switchToLayout(QAction *action)
{
    const QString layout = action->data().toString();

    if (layout == QLatin1String(SHOW_SETTINGS_ACTION)) {
        QTimer::singleShot(400, [this]() {
            QDBusInterface iface = syndockInterface();

            if (iface.isValid()) {
                iface.call("showSettingsWindow", (int)LayoutPage);
            }
        });
    } else {
        QTimer::singleShot(400, [this, layout]() {
            QDBusInterface iface = syndockInterface();

            if (iface.isValid()) {
                iface.call("switchToLayout", layout);
            }
        });
    }
}

void Menu::quitApplication()
{
    QDBusInterface iface = syndockInterface();

    if (iface.isValid()) {
        iface.call("quitApplication");
    }
}

K_PLUGIN_CLASS_WITH_JSON(Menu, "plasma-containmentactions-lattecontextmenu.json")

#include "menu.moc"
