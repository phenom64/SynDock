# SynDock Context Menu
# Copyright (C) 2026 Syndromatic Ltd.

add_definitions(-DTRANSLATION_DOMAIN=\"plasma_containmentactions_syndockcontextmenu\")

set(contextmenu_SRCS
    generictools.cpp
    layoutmenuitemwidget.cpp
    menu.cpp
)

add_library(plasma_containmentactions_syndockcontextmenu MODULE ${contextmenu_SRCS})

target_link_libraries(plasma_containmentactions_syndockcontextmenu
    Qt6::DBus
    Qt6::Widgets
    KF6::I18n
    KF6::XmlGui
    Plasma::Plasma
)

install(TARGETS plasma_containmentactions_syndockcontextmenu DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/containmentactions)
