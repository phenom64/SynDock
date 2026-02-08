# SynDock Plasmoid (Tasks Widget)
# Copyright (C) 2026 Syndromatic Ltd.

add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.syndock.plasmoid\")

# Install Plasma package using KF6 syntax
kpackage_install_package(package org.kde.syndock.plasmoid plasma plasmoids)

set(tasks_SRCS
    plugin/types.cpp
    plugin/syndocktasksplugin.cpp
)

add_library(syndocktasksplugin SHARED ${tasks_SRCS})

target_link_libraries(syndocktasksplugin
    Qt6::Core
    Qt6::Qml
    Plasma::Plasma
    Plasma::PlasmaQuick
)
                      
install(TARGETS syndocktasksplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/syndock/private/tasks)
install(FILES plugin/qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/syndock/private/tasks)
