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

// local
#include "config-syndock.h"
#include "apptypes.h"
#include "NSETypes.h"
#include "nsecoronainterface.h"
#include "layouts/importer.h"
#include "templates/templatesmanager.h"

// C++
#include <memory>
#include <csignal>

// Qt
#include <QApplication>
#include <QDebug>
#include <QQuickWindow>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDir>
#include <QFile>
#include <QLockFile>
#include <QSessionManager>
#include <QSharedMemory>
#include <QTextStream>

// KDE
#include <KCrash>
#include <KLocalizedString>
#include <KAboutData>
#include <KDBusService>

//! COLORS
#define CNORMAL  "\e[0m"
#define CIGREEN  "\e[1;32m"
#define CGREEN   "\e[0;32m"
#define CICYAN   "\e[1;36m"
#define CCYAN    "\e[0;36m"
#define CIRED    "\e[1;31m"
#define CRED     "\e[0;31m"

inline void configureAboutData();
inline void detectPlatform(int argc, char **argv);
inline void filterDebugMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

QString filterDebugMessageText;
QString filterDebugLogFile;

int main(int argc, char **argv)
{
    // Qt 6: Native HiDPI scaling - no manual AA_ attributes needed
    // The deprecated Qt::AA_DisableHighDpiScaling and Qt::AA_UseHighDpiPixmaps
    // are removed in Qt 6; scaling is handled automatically via QT_SCALE_FACTOR
    
    QQuickWindow::setDefaultAlphaBuffer(true);

    // Wayland-only platform detection
    detectPlatform(argc, argv);
    
    QApplication app(argc, argv);

    // KF6: KQuickAddons::QtQuickSettings removed; Qt 6 handles quick settings natively

    KLocalizedString::setApplicationDomain("syndock");
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("syndock")));
    // Protect from closing app when changing to "alternative session" and back
    app.setQuitOnLastWindowClosed(false);

    configureAboutData();

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOptions({
                          {{"r", "replace"}, i18nc("command line", "Replace the current SynDock instance.")}
                          , {{"d", "debug"}, i18nc("command line", "Show the debugging messages on stdout.")}
                          , {{"cc", "clear-cache"}, i18nc("command line", "Clear qml cache. It can be useful after system upgrades.")}
                          , {"enable-autostart", i18nc("command line", "Enable autostart for this application")}
                          , {"disable-autostart", i18nc("command line", "Disable autostart for this application")}
                          , {"default-layout", i18nc("command line", "Import and load default layout on startup.")}
                          , {"available-layouts", i18nc("command line", "Print available layouts")}
                          , {"available-dock-templates", i18nc("command line", "Print available dock templates")}
                          , {"available-layout-templates", i18nc("command line", "Print available layout templates")}
                          , {"layout", i18nc("command line", "Load specific layout on startup."), i18nc("command line: load", "layout_name")}
                          , {"import-layout", i18nc("command line", "Import and load a layout."), i18nc("command line: import", "absolute_filepath")}
                          , {"suggested-layout-name", i18nc("command line", "Suggested layout name when importing a layout file"), i18nc("command line: import", "suggested_name")}
                          , {"import-full", i18nc("command line", "Import full configuration."), i18nc("command line: import", "file_name")}
                          , {"add-dock", i18nc("command line", "Add Dock/Panel"), i18nc("command line: add", "template_name")}
                          , {"single", i18nc("command line", "Single layout memory mode. Only one layout is active at any case.")}
                          , {"multiple", i18nc("command line", "Multiple layouts memory mode. Multiple layouts can be active at any time based on Activities running.")}
                      });

    //! START: Hidden options for Developer and Debugging usage
    QCommandLineOption graphicsOption(QStringList() << QStringLiteral("graphics"));
    graphicsOption.setDescription(QStringLiteral("Draw boxes around of the applets."));
    graphicsOption.setFlags(QCommandLineOption::HiddenFromHelp);
    parser.addOption(graphicsOption);

    QCommandLineOption withWindowOption(QStringList() << QStringLiteral("with-window"));
    withWindowOption.setDescription(QStringLiteral("Open a window with much debug information"));
    withWindowOption.setFlags(QCommandLineOption::HiddenFromHelp);
    parser.addOption(withWindowOption);

    QCommandLineOption maskOption(QStringList() << QStringLiteral("mask"));
    maskOption.setDescription(QStringLiteral("Show messages of debugging for the mask (Only useful to devs)."));
    maskOption.setFlags(QCommandLineOption::HiddenFromHelp);
    parser.addOption(maskOption);

    QCommandLineOption timersOption(QStringList() << QStringLiteral("timers"));
    timersOption.setDescription(QStringLiteral("Show messages for debugging the timers (Only useful to devs)."));
    timersOption.setFlags(QCommandLineOption::HiddenFromHelp);
    parser.addOption(timersOption);

    QCommandLineOption spacersOption(QStringList() << QStringLiteral("spacers"));
    spacersOption.setDescription(QStringLiteral("Show visual indicators for debugging spacers (Only useful to devs)."));
    spacersOption.setFlags(QCommandLineOption::HiddenFromHelp);
    parser.addOption(spacersOption);

    QCommandLineOption overloadedIconsOption(QStringList() << QStringLiteral("overloaded-icons"));
    overloadedIconsOption.setDescription(QStringLiteral("Show visual indicators for debugging overloaded applets icons (Only useful to devs)."));
    overloadedIconsOption.setFlags(QCommandLineOption::HiddenFromHelp);
    parser.addOption(overloadedIconsOption);

    QCommandLineOption edgesOption(QStringList() << QStringLiteral("kwinedges"));
    edgesOption.setDescription(QStringLiteral("Show visual window indicators for hidden screen edge windows."));
    edgesOption.setFlags(QCommandLineOption::HiddenFromHelp);
    parser.addOption(edgesOption);

    QCommandLineOption localGeometryOption(QStringList() << QStringLiteral("localgeometry"));
    localGeometryOption.setDescription(QStringLiteral("Show visual window indicators for calculated local geometry."));
    localGeometryOption.setFlags(QCommandLineOption::HiddenFromHelp);
    parser.addOption(localGeometryOption);

    QCommandLineOption layouterOption(QStringList() << QStringLiteral("layouter"));
    layouterOption.setDescription(QStringLiteral("Show visual debug tags for items sizes."));
    layouterOption.setFlags(QCommandLineOption::HiddenFromHelp);
    parser.addOption(layouterOption);

    QCommandLineOption filterDebugTextOption(QStringList() << QStringLiteral("debug-text"));
    filterDebugTextOption.setDescription(QStringLiteral("Show only debug messages that contain specific text."));
    filterDebugTextOption.setFlags(QCommandLineOption::HiddenFromHelp);
    filterDebugTextOption.setValueName(i18nc("command line: debug-text", "filter_debug_text"));
    parser.addOption(filterDebugTextOption);

    QCommandLineOption filterDebugInputMask(QStringList() << QStringLiteral("input"));
    filterDebugInputMask.setDescription(QStringLiteral("Show visual window indicators for calculated input mask."));
    filterDebugInputMask.setFlags(QCommandLineOption::HiddenFromHelp);
    parser.addOption(filterDebugInputMask);

    QCommandLineOption filterDebugEventSinkMask(QStringList() << QStringLiteral("events-sink"));
    filterDebugEventSinkMask.setDescription(QStringLiteral("Show visual indicators for areas of EventsSink."));
    filterDebugEventSinkMask.setFlags(QCommandLineOption::HiddenFromHelp);
    parser.addOption(filterDebugEventSinkMask);

    QCommandLineOption filterDebugLogCmd(QStringList() << QStringLiteral("log-file"));
    filterDebugLogCmd.setDescription(QStringLiteral("Forward debug output in a log text file."));
    filterDebugLogCmd.setFlags(QCommandLineOption::HiddenFromHelp);
    filterDebugLogCmd.setValueName(i18nc("command line: log-filepath", "filter_log_filepath"));
    parser.addOption(filterDebugLogCmd);
    //! END: Hidden options

    parser.process(app);

    if (parser.isSet(QStringLiteral("enable-autostart"))) {
        NSE::Layouts::Importer::enableAutostart();
    }

    if (parser.isSet(QStringLiteral("disable-autostart"))) {
        NSE::Layouts::Importer::disableAutostart();
        qGuiApp->exit();
        return 0;
    }

    //! print available-layouts
    if (parser.isSet(QStringLiteral("available-layouts"))) {
        QStringList layouts = NSE::Layouts::Importer::availableLayouts();

        if (layouts.count() > 0) {
            qInfo() << i18n("Available layouts that can be used to start SynDock:");

            for (const auto &layout : layouts) {
                qInfo() << "     " << layout;
            }
        } else {
            qInfo() << i18n("There are no available layouts, during startup Default will be used.");
        }

        qGuiApp->exit();
        return 0;
    }

    //! print available-layout-templates
    if (parser.isSet(QStringLiteral("available-layout-templates"))) {
        QStringList templates = NSE::Layouts::Importer::availableLayoutTemplates();

        if (templates.count() > 0) {
            qInfo() << i18n("Available layout templates found in your system:");

            for (const auto &templatename : templates) {
                qInfo() << "     " << templatename;
            }
        } else {
            qInfo() << i18n("There are no available layout templates in your system.");
        }

        qGuiApp->exit();
        return 0;
    }

    //! print available-dock-templates
    if (parser.isSet(QStringLiteral("available-dock-templates"))) {
        QStringList templates = NSE::Layouts::Importer::availableViewTemplates();

        if (templates.count() > 0) {
            qInfo() << i18n("Available dock templates found in your system:");

            for (const auto &templatename : templates) {
                qInfo() << "     " << templatename;
            }
        } else {
            qInfo() << i18n("There are no available dock templates in your system.");
        }

        qGuiApp->exit();
        return 0;
    }

    //! disable restore from session management
    //! based on spectacle solution at:
    //!   - https://bugs.kde.org/show_bug.cgi?id=430411
    //!   - https://invent.kde.org/graphics/spectacle/-/commit/8db27170d63f8a4aaff09615e51e3cc0fb115c4d
    QGuiApplication::setFallbackSessionManagementEnabled(false);

    auto disableSessionManagement = [](QSessionManager &sm) {
        sm.setRestartHint(QSessionManager::RestartNever);
    };
    QObject::connect(&app, &QGuiApplication::commitDataRequest, disableSessionManagement);
    QObject::connect(&app, &QGuiApplication::saveStateRequest, disableSessionManagement);

    //! choose layout for startup
    bool defaultLayoutOnStartup = false;
    int memoryUsage = -1;
    QString layoutNameOnStartup = "";
    QString addViewTemplateNameOnStartup = "";

    //! --default-layout option
    if (parser.isSet(QStringLiteral("default-layout"))) {
        defaultLayoutOnStartup = true;
    } else if (parser.isSet(QStringLiteral("layout"))) {
        layoutNameOnStartup = parser.value(QStringLiteral("layout"));

        if (!NSE::Layouts::Importer::layoutExists(layoutNameOnStartup)) {
            qInfo() << i18nc("layout missing", "This layout doesn't exist in the system.");
            qGuiApp->exit();
            return 0;
        }
    }

    //! --replace option
    QString username = qgetenv("USER");

    if (username.isEmpty())
        username = qgetenv("USERNAME");

    QLockFile lockFile {QDir::tempPath() + "/syndock." + username + ".lock"};

    int timeout {100};

    if (parser.isSet(QStringLiteral("replace")) || parser.isSet(QStringLiteral("import-full"))) {
        qint64 pid{ -1};

        if (lockFile.getLockInfo(&pid, nullptr, nullptr)) {
            kill(static_cast<pid_t>(pid), SIGINT);
            timeout = -1;
        }
    }

    if (!lockFile.tryLock(timeout)) {
        QDBusInterface iface("org.syndromatic.syndock", "/SynDock", "org.syndromatic.SynDock", QDBusConnection::sessionBus());
        bool addview{parser.isSet(QStringLiteral("add-dock"))};
        bool importlayout{parser.isSet(QStringLiteral("import-layout"))};
        bool enableautostart{parser.isSet(QStringLiteral("enable-autostart"))};
        bool disableautostart{parser.isSet(QStringLiteral("disable-autostart"))};

        bool validaction{false};

        if (iface.isValid()) {
            if (addview) {
                validaction = true;
                iface.call("addView", (uint)0, parser.value(QStringLiteral("add-dock")));
                qGuiApp->exit();
                return 0;
            } else if (importlayout) {
                validaction = true;
                QString suggestedname = parser.isSet(QStringLiteral("suggested-layout-name")) ? parser.value(QStringLiteral("suggested-layout-name")) : QString();
                iface.call("importLayoutFile", parser.value(QStringLiteral("import-layout")), suggestedname);
                qGuiApp->exit();
                return 0;
            } else if (enableautostart || disableautostart){
                validaction = true;
            } else {
                // LayoutPage = 0
                iface.call("showSettingsWindow", 0);
            }
        }

        if (!validaction) {
            qInfo() << i18n("An instance is already running! Use --replace to restart SynDock.");
        }

        qGuiApp->exit();
        return 0;
    }

    //! clear-cache option
    if (parser.isSet(QStringLiteral("clear-cache"))) {
        QDir cacheDir(QDir::homePath() + "/.cache/syndock/qmlcache");

        if (cacheDir.exists()) {
            cacheDir.removeRecursively();
            qDebug() << "Cache directory found and cleared...";
        }
    }

    //! import-full option
    if (parser.isSet(QStringLiteral("import-full"))) {
        bool imported = NSE::Layouts::Importer::importHelper(parser.value(QStringLiteral("import-full")));

        if (!imported) {
            qInfo() << i18n("The configuration cannot be imported");
            qGuiApp->exit();
            return 0;
        }
    }

    //! import-layout option
    if (parser.isSet(QStringLiteral("import-layout"))) {
        QString suggestedname = parser.isSet(QStringLiteral("suggested-layout-name")) ? parser.value(QStringLiteral("suggested-layout-name")) : QString();
        QString importedLayout = NSE::Layouts::Importer::importLayoutHelper(parser.value(QStringLiteral("import-layout")), suggestedname);

        if (importedLayout.isEmpty()) {
            qInfo() << i18n("The layout cannot be imported");
            qGuiApp->exit();
            return 0;
        } else {
            layoutNameOnStartup = importedLayout;
        }
    }

    //! memory usage option
    if (parser.isSet(QStringLiteral("multiple"))) {
        memoryUsage = (int)(NSE::MemoryUsage::MultipleLayouts);
    } else if (parser.isSet(QStringLiteral("single"))) {
        memoryUsage = (int)(NSE::MemoryUsage::SingleLayout);
    }

    //! add-dock usage option
    if (parser.isSet(QStringLiteral("add-dock"))) {
        QString viewTemplateName = parser.value(QStringLiteral("add-dock"));
        QStringList viewTemplates = NSE::Layouts::Importer::availableViewTemplates();

        if (viewTemplates.contains(viewTemplateName)) {
            if (layoutNameOnStartup.isEmpty()) {
                //! Clean layout template is applied and proper name is used
                QString emptytemplatepath = NSE::Layouts::Importer::layoutTemplateSystemFilePath(NSE::Templates::EMPTYLAYOUTTEMPLATENAME);
                QString suggestedname = parser.isSet(QStringLiteral("suggested-layout-name")) ? parser.value(QStringLiteral("suggested-layout-name")) : viewTemplateName;
                QString importedLayout = NSE::Layouts::Importer::importLayoutHelper(emptytemplatepath, suggestedname);

                if (importedLayout.isEmpty()) {
                    qInfo() << i18n("The layout cannot be imported");
                    qGuiApp->exit();
                    return 0;
                } else {
                    layoutNameOnStartup = importedLayout;
                }
            }

            addViewTemplateNameOnStartup = viewTemplateName;
        }
    }

    //! text filter for debug messages
    if (parser.isSet(QStringLiteral("debug-text"))) {
        filterDebugMessageText = parser.value(QStringLiteral("debug-text"));
    }

    //! log file for debug output
    if (parser.isSet(QStringLiteral("log-file")) && !parser.value(QStringLiteral("log-file")).isEmpty()) {
        filterDebugLogFile = parser.value(QStringLiteral("log-file"));
    }

    //! debug/mask options
    if (parser.isSet(QStringLiteral("debug")) || parser.isSet(QStringLiteral("mask")) || parser.isSet(QStringLiteral("debug-text"))) {
        qInstallMessageHandler(filterDebugMessageOutput);
    } else {
        const auto noMessageOutput = [](QtMsgType, const QMessageLogContext &, const QString &) {};
        qInstallMessageHandler(noMessageOutput);
    }

    auto signal_handler = [](int) {
        qGuiApp->exit();
    };

    std::signal(SIGKILL, signal_handler);
    std::signal(SIGINT, signal_handler);

    KCrash::setDrKonqiEnabled(true);
    KCrash::setFlags(KCrash::AutoRestart | KCrash::AlwaysDirectly);

    NSE::Corona corona(defaultLayoutOnStartup, layoutNameOnStartup, addViewTemplateNameOnStartup, memoryUsage);
    KDBusService service(KDBusService::Unique);

    return app.exec();
}

inline void filterDebugMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (msg.endsWith("QML Binding: Not restoring previous value because restoreMode has not been set.This behavior is deprecated.In Qt < 6.0 the default is Binding.RestoreBinding.In Qt >= 6.0 the default is Binding.RestoreBindingOrValue.")
        || msg.endsWith("QML Binding: Not restoring previous value because restoreMode has not been set.\nThis behavior is deprecated.\nYou have to import QtQml 2.15 after any QtQuick imports and set\nthe restoreMode of the binding to fix this warning.\nIn Qt < 6.0 the default is Binding.RestoreBinding.\nIn Qt >= 6.0 the default is Binding.RestoreBindingOrValue.\n")
        || msg.endsWith("QML Binding: Not restoring previous value because restoreMode has not been set.\nThis behavior is deprecated.\nYou have to import QtQml 2.15 after any QtQuick imports and set\nthe restoreMode of the binding to fix this warning.\nIn Qt < 6.0 the default is Binding.RestoreBinding.\nIn Qt >= 6.0 the default is Binding.RestoreBindingOrValue.")
        || msg.endsWith("QML Connections: Implicitly defined onFoo properties in Connections are deprecated. Use this syntax instead: function onFoo(<arguments>) { ... }")) {
        //! block warnings because they will be needed only after qt6.0 support. Currently Binding.restoreMode can not be supported because
        //! qt5.9 is the minimum supported version.
        return;
    }

    if (!filterDebugMessageText.isEmpty() && !msg.contains(filterDebugMessageText)) {
        return;
    }

    const char *function = context.function ? context.function : "";

    QString typeStr;
    switch (type) {
    case QtDebugMsg:
        typeStr = "Debug";
        break;
    case QtInfoMsg:
        typeStr = "Info";
        break;
    case QtWarningMsg:
        typeStr = "Warning" ;
        break;
    case QtCriticalMsg:
        typeStr = "Critical";
        break;
    case QtFatalMsg:
        typeStr = "Fatal";
        break;
    };

    const char *TypeColor;

    if (type == QtInfoMsg || type == QtWarningMsg) {
        TypeColor = CGREEN;
    } else if (type == QtCriticalMsg || type == QtFatalMsg) {
        TypeColor = CRED;
    } else {
        TypeColor = CIGREEN;
    }

    if (filterDebugLogFile.isEmpty()) {
        qDebug().nospace() << TypeColor << "[" << typeStr.toStdString().c_str() << " : " << CGREEN << QTime::currentTime().toString("h:mm:ss.zz").toStdString().c_str() << TypeColor << "]" << CNORMAL
                          #ifndef QT_NO_DEBUG
                           << CIRED << " [" << CCYAN << function << CIRED << ":" << CCYAN << context.line << CIRED << "]"
                          #endif
                           << CICYAN << " - " << CNORMAL << msg;
    } else {
        QFile logfile(filterDebugLogFile);
        logfile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream logts(&logfile);
        logts << "[" << typeStr.toStdString().c_str() << " : " << QTime::currentTime().toString("h:mm:ss.zz").toStdString().c_str() << "]"
              <<  " - " << msg << Qt::endl;
    }
}

inline void configureAboutData()
{
    KAboutData about(QStringLiteral("syndock")
                     , QStringLiteral("SynDock")
                     , QStringLiteral(VERSION)
                     , i18n("SynDock is the primary docking interface for the Syndromatic Desktop Experience. "
                            "It features the NSE Parabolic Zoom effect, live window previews, and deep "
                            "integration with KDE Plasma 6.\n\n\"The Syndromatic Experience\"")
                     , KAboutLicense::GPL_V2
                     , QStringLiteral("\251 2026 Syndromatic Ltd. All rights reserved"));

    about.setHomepage(WEBSITE);
    about.setProgramLogo(QIcon::fromTheme(QStringLiteral("syndock")));
    about.setDesktopFileName(QStringLiteral("org.syndromatic.syndock"));
    about.setProductName(QByteArray("syndock"));

    // Authors
    about.addAuthor(QStringLiteral("Kavish Krishnakumar"), i18n("Lead Developer"), QStringLiteral("kavish@syndromatic.com"));
    
    // Original Latte Dock authors (credit retained)
    about.addCredit(QStringLiteral("Michail Vourlakos"), i18n("Original Latte Dock author"), QStringLiteral("mvourlakos@gmail.com"));
    about.addCredit(QStringLiteral("Smith AR"), i18n("Original Latte Dock author"), QStringLiteral("audoban@openmailbox.org"));

    KAboutData::setApplicationData(about);
}

// Wayland-only platform detection
// X11 support has been removed from SynDock
inline void detectPlatform([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    if (qEnvironmentVariableIsSet("QT_QPA_PLATFORM")) {
        return;
    }

    const QByteArray sessionType = qgetenv("XDG_SESSION_TYPE");

    if (sessionType.isEmpty() || qstrcmp(sessionType, "wayland") == 0) {
        qputenv("QT_QPA_PLATFORM", "wayland");
    } else {
        // SynDock is Wayland-only; warn if running under X11
        qWarning() << "SynDock requires a Wayland session. X11 is not supported.";
        qputenv("QT_QPA_PLATFORM", "wayland");
    }
}
