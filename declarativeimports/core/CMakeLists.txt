# SynDock Core Plugin
# Copyright (C) 2026 Syndromatic Ltd.

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-latte-lib.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-latte-lib.h)

set(syndockcoreplugin_SRCS
    syndockcoreplugin.cpp
    dialog.cpp
    environment.cpp
    iconitem.cpp
    quickwindowsystem.cpp
    tools.cpp
    types.h
)

add_library(syndockcoreplugin SHARED ${syndockcoreplugin_SRCS})

target_link_libraries(syndockcoreplugin
    Qt6::Quick
    Qt6::Qml
    KF6::CoreAddons
    ${SYNDOCK_KF6_DECLARATIVE_LINK}
    KF6::IconThemes
    KF6::WindowSystem
    Plasma::Plasma
    Plasma::PlasmaQuick
)

if(NOT "${SYNDOCK_KF6_DECLARATIVE_INCLUDE_DIRS}" STREQUAL "")
    target_include_directories(syndockcoreplugin PRIVATE ${SYNDOCK_KF6_DECLARATIVE_INCLUDE_DIRS})
endif()

install(TARGETS syndockcoreplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/syndock/core)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/syndock/core)
