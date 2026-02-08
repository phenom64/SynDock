/*
    SPDX-FileCopyrightText: 2016 Smith AR <audoban@openmailbox.org>
    SPDX-FileCopyrightText: 2016 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef GLOBALSHORTCUTS_H
#define GLOBALSHORTCUTS_H

// local
#include <coretypes.h>

// Qt
#include <QAction>
#include <QPointer>
#include <QTimer>

// KDE
#include <kmodifierkeyinfo.h>


namespace Plasma {
class Containment;
}

namespace NSE {
class Corona;
class View;
namespace ShortcutsPart{
class ModifierTracker;
class ShortcutsTracker;
}
}

namespace NSE {

class GlobalShortcuts : public QObject
{
    Q_OBJECT

public:
    static constexpr const char* SHORTCUTBLOCKHIDINGTYPE  = "globalshortcuts::blockHiding()";

    GlobalShortcuts(QObject *parent = nullptr);
    ~GlobalShortcuts() override;

    void activateLauncherMenu();
    void updateViewItemBadge(QString identifier, QString value);

    ShortcutsPart::ShortcutsTracker *shortcutsTracker() const;

signals:
    void modifiersChanged();

private slots:
    void hideViewsTimerSlot();

private:
    void init();
    void initModifiers();
    void activateEntry(int index, Qt::Key modifier);
    void showViews();
    void showSettings();

    bool activateLatteEntry(NSE::View *view, int index, Qt::Key modifier, bool *delayedExecution);
    bool activatePlasmaTaskManager(const NSE::View *view, int index, Qt::Key modifier, bool *delayedExecution);
    bool activateEntryForView(NSE::View *view, int index, Qt::Key modifier);
    bool viewAtLowerEdgePriority(NSE::View *test, NSE::View *base);
    bool viewAtLowerScreenPriority(NSE::View *test, NSE::View *base);
    bool viewsToHideAreValid();

    //! highest priority application launcher view
    NSE::View *highestApplicationLauncherView(const QList<NSE::View *> &views) const;

    QList<NSE::View *> sortedViewsList(QHash<const Plasma::Containment *, NSE::View *> *views);

private:
    bool m_metaShowedViews{false};

    //! last action that was triggered from the user
    QAction *m_lastInvokedAction;
    //! it is used for code compatibility reasons in order to replicate a single Meta action
    QAction *m_singleMetaAction;

    //! delayer for hiding the shown latte views
    QTimer m_hideViewsTimer;
    QList<NSE::View *> m_hideViews;

    QPointer<ShortcutsPart::ModifierTracker> m_modifierTracker;
    QPointer<ShortcutsPart::ShortcutsTracker> m_shortcutsTracker;
    QPointer<NSE::Corona> m_corona;
};

}

#endif // GLOBALSHORTCUTS_H
