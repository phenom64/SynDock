add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.syndromatic.syndock.plasmoid\")

plasma_install_package(package org.syndromatic.syndock.plasmoid)

set(tasks_SRCS
    plugin/types.cpp
    plugin/syndocktasksplugin.cpp
)

qt_add_library(syndocktasksplugin SHARED ${tasks_SRCS})

target_link_libraries(syndocktasksplugin
                      Qt::Core
                      Qt::Qml
                      Plasma::Plasma
                      Plasma::PlasmaQuick)

install(TARGETS syndocktasksplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/syndromatic/syndock/private/tasks)
install(FILES plugin/qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/syndromatic/syndock/private/tasks)
