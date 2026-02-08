# SynDock Application
# Copyright (C) 2026 Syndromatic Ltd. All rights reserved
# SPDX-License-Identifier: GPL-2.0-or-later

set(syndock-app_SRCS
    NSETypes.h
    alternativeshelper.cpp
    apptypes.cpp
    infoview.cpp
    nsecoronainterface.cpp
    screenpool.cpp
    primaryoutputwatcher.cpp
    main.cpp
    coretypes.h
)

add_subdirectory(data)
add_subdirectory(declarativeimports)
add_subdirectory(indicator)
add_subdirectory(layout)
add_subdirectory(layouts)
add_subdirectory(package)
add_subdirectory(plasma/extended)
add_subdirectory(settings)
add_subdirectory(settings/generic)
add_subdirectory(settings/actionsdialog)
add_subdirectory(settings/detailsdialog)
add_subdirectory(settings/detailsdialog/delegates)
add_subdirectory(settings/exporttemplatedialog)
add_subdirectory(settings/exporttemplatedialog/delegates)
add_subdirectory(settings/screensdialog)
add_subdirectory(settings/screensdialog/delegates)
add_subdirectory(settings/settingsdialog)
add_subdirectory(settings/settingsdialog/delegates)
add_subdirectory(settings/viewsdialog)
add_subdirectory(settings/viewsdialog/delegates)
add_subdirectory(shortcuts)
add_subdirectory(templates)
add_subdirectory(tools)
add_subdirectory(view)
add_subdirectory(view/helpers)
add_subdirectory(view/indicator)
add_subdirectory(view/settings)
add_subdirectory(view/windowstracker)
add_subdirectory(wm)
add_subdirectory(wm/tracker)

# D-Bus interface - SynDock uses org.syndromatic.SynDock
set(syndock_dbusXML dbus/org.syndromatic.SynDock.xml)
qt6_add_dbus_adaptor(syndock-app_SRCS ${syndock_dbusXML} nsecoronainterface.h NSE::Corona syndockadaptor)

# UI files
ki18n_wrap_ui(syndock-app_SRCS settings/actionsdialog/actionsdialog.ui)
ki18n_wrap_ui(syndock-app_SRCS settings/detailsdialog/detailsdialog.ui)
ki18n_wrap_ui(syndock-app_SRCS settings/exporttemplatedialog/exporttemplatedialog.ui)
ki18n_wrap_ui(syndock-app_SRCS settings/screensdialog/screensdialog.ui)
ki18n_wrap_ui(syndock-app_SRCS settings/settingsdialog/settingsdialog.ui)
ki18n_wrap_ui(syndock-app_SRCS settings/viewsdialog/viewsdialog.ui)

# Create the executable first
add_executable(syndock ${syndock-app_SRCS})

# Wayland protocols (must be after add_executable in Qt 6)
qt6_generate_wayland_protocol_client_sources(syndock
    FILES ${PLASMA_WAYLAND_PROTOCOLS_DIR}/kde-primary-output-v1.xml
)

include(FakeTarget.cmake)

# Resolve KWayland client linkage across distro packaging variants.
set(SYNDOCK_KWAYLAND_CLIENT_TARGET "")
set(SYNDOCK_KWAYLAND_CLIENT_LIBRARIES "")
set(SYNDOCK_KWAYLAND_CLIENT_INCLUDE_DIRS "")
if(TARGET KWayland::Client)
    set(SYNDOCK_KWAYLAND_CLIENT_TARGET KWayland::Client)
elseif(TARGET KF6::WaylandClient)
    set(SYNDOCK_KWAYLAND_CLIENT_TARGET KF6::WaylandClient)
elseif(TARGET KF6::KWaylandClient)
    set(SYNDOCK_KWAYLAND_CLIENT_TARGET KF6::KWaylandClient)
elseif(DEFINED KWayland_LIBRARIES AND NOT "${KWayland_LIBRARIES}" STREQUAL "")
    set(SYNDOCK_KWAYLAND_CLIENT_LIBRARIES ${KWayland_LIBRARIES})
    if(DEFINED KWayland_INCLUDE_DIRS)
        set(SYNDOCK_KWAYLAND_CLIENT_INCLUDE_DIRS ${KWayland_INCLUDE_DIRS})
    endif()
endif()

if(SYNDOCK_KWAYLAND_CLIENT_TARGET STREQUAL "" AND "${SYNDOCK_KWAYLAND_CLIENT_LIBRARIES}" STREQUAL "")
    message(FATAL_ERROR "SynDock requires KWayland client development files (target KWayland::Client/KF6::WaylandClient or KWayland_LIBRARIES from find_package).")
endif()

# Resolve activities target across framework variants.
set(SYNDOCK_ACTIVITIES_TARGET "")
if(TARGET Plasma::Activities)
    set(SYNDOCK_ACTIVITIES_TARGET Plasma::Activities)
elseif(TARGET KF6::Activities)
    set(SYNDOCK_ACTIVITIES_TARGET KF6::Activities)
elseif(TARGET KActivities::KActivities)
    set(SYNDOCK_ACTIVITIES_TARGET KActivities::KActivities)
endif()

if(SYNDOCK_ACTIVITIES_TARGET STREQUAL "")
    message(FATAL_ERROR "SynDock requires an activities target (Plasma::Activities, KF6::Activities or KActivities::KActivities)")
endif()

# Qt 6 / KF6 libraries - Wayland only (no X11)
target_link_libraries(syndock
    # Qt 6
    Qt6::Core
    Qt6::DBus
    Qt6::Quick
    Qt6::Qml
    Qt6::QuickControls2
    Qt6::WaylandClient
    
    # KDE Frameworks 6
    KF6::Archive
    KF6::CoreAddons
    KF6::Crash
    KF6::DBusAddons
    KF6::GuiAddons
    KF6::GlobalAccel
    KF6::I18n
    KF6::IconThemes
    KF6::KIOWidgets
    KF6::Notifications
    KF6::Svg
    KF6::WindowSystem
    KF6::XmlGui
    
    # Plasma 6
    Plasma::Plasma
    Plasma::PlasmaQuick
    ${SYNDOCK_ACTIVITIES_TARGET}
    
    # Wayland
    Wayland::Client
    ${SYNDOCK_KWAYLAND_CLIENT_TARGET}
    ${SYNDOCK_KWAYLAND_CLIENT_LIBRARIES}
    LayerShellQt::Interface
)

if(NOT "${SYNDOCK_KWAYLAND_CLIENT_INCLUDE_DIRS}" STREQUAL "")
    target_include_directories(syndock PRIVATE ${SYNDOCK_KWAYLAND_CLIENT_INCLUDE_DIRS})
endif()

# Desktop files - renamed from latte-dock to syndock
configure_file(org.kde.latte-dock.desktop.cmake org.syndromatic.syndock.desktop)
configure_file(org.kde.latte-dock.appdata.xml.cmake org.syndromatic.syndock.appdata.xml)

install(TARGETS syndock ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.syndromatic.syndock.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.syndromatic.syndock.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES dbus/org.syndromatic.SynDock.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
install(FILES lattedock.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
install(FILES latte-layouts.knsrc DESTINATION ${KDE_INSTALL_DATADIR}/knsrcfiles)
install(FILES latte-indicators.knsrc DESTINATION ${KDE_INSTALL_DATADIR}/knsrcfiles)

add_subdirectory(packageplugins)
