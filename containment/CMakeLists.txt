add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.syndromatic.syndock.containment\")

plasma_install_package(package org.syndromatic.syndock.containment)

set(containment_SRCS
    plugin/nsetypes.h
    plugin/types.cpp
    plugin/layoutmanager.cpp
    plugin/syndockcontainmentplugin.cpp
)

add_library(syndockcontainmentplugin SHARED ${containment_SRCS})

target_link_libraries(syndockcontainmentplugin
                      Qt::Core
                      Qt::Qml
                      Qt::Quick
                      KF6::ConfigQml
                      Plasma::Plasma
                      Plasma::PlasmaQuick)

install(TARGETS syndockcontainmentplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/syndromatic/syndock/private/containment)
install(FILES plugin/qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/syndromatic/syndock/private/containment)
