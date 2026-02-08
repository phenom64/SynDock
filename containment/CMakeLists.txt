# SynDock Containment
# Copyright (C) 2026 Syndromatic Ltd.

add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.syndock.containment\")

# Install Plasma package using KF6 syntax
kpackage_install_package(package org.kde.syndock.containment plasma plasmoids)

set(containment_SRCS
    plugin/types.h
    plugin/types.cpp
    plugin/layoutmanager.cpp
    plugin/syndockcontainmentplugin.cpp
)

add_library(syndockcontainmentplugin SHARED ${containment_SRCS})

target_link_libraries(syndockcontainmentplugin
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    KF6::Declarative
    Plasma::Plasma
    Plasma::PlasmaQuick
)

install(TARGETS syndockcontainmentplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/syndock/private/containment)
install(FILES plugin/qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/syndock/private/containment)
