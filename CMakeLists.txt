# SynDock - The NSE Docking Experience for SynOS
# Copyright (C) 2026 Syndromatic Ltd. All rights reserved
# Designed by Kavish Krishnakumar in Manchester.
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Based on Latte Dock
# SPDX-FileCopyrightText: 2016 Smith AR <audoban@openmailbox.org>
# SPDX-FileCopyrightText: 2016 Michail Vourlakos <mvourlakos@gmail.com>

cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(syndock VERSION 1.0.0 LANGUAGES CXX)

# Modern C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project metadata
set(AUTHOR "Syndromatic Ltd.")
set(EMAIL "support@syndromatic.com")
set(WEBSITE "https://syndromatic.com/syndock")
set(BUG_ADDRESS "https://github.com/syndromatic/syndock/issues")

# Version requirements - Qt 6 / KF6 / Plasma 6
set(QT_MIN_VERSION "6.7.0")
set(KF6_MIN_VERSION "6.0.0")
set(PLASMA_MIN_VERSION "6.0.0")

# Find ECM (Extra CMake Modules)
find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

# KDE CMake includes
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMOptionalAddSubdirectory)
include(ECMQtDeclareLoggingCategory)
include(CMakePackageConfigHelpers)
include(CheckIncludeFiles)
include(FeatureSummary)
include(FindPkgConfig)

# ============================================================================
# Qt 6 - Wayland-only build (no X11 dependencies)
# ============================================================================
find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    DBus
    Gui
    Qml
    Quick
    QuickControls2
    Widgets
    WaylandClient
)

# ============================================================================
# KDE Frameworks 6
# ============================================================================
find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    Archive
    CoreAddons
    Declarative
    GuiAddons
    Crash
    DBusAddons
    GlobalAccel
    I18n
    IconThemes
    KIO
    Notifications
    Package
    Svg
    WindowSystem
    XmlGui
)

# Some distributions expose KDeclarative with different target exports.
find_package(KF6Declarative REQUIRED)
set(SYNDOCK_KF6_DECLARATIVE_LINK "")
set(SYNDOCK_KF6_DECLARATIVE_INCLUDE_DIRS "")

if(TARGET KF6::Declarative)
    set(SYNDOCK_KF6_DECLARATIVE_LINK KF6::Declarative)
elseif(TARGET KF6::KDeclarative)
    set(SYNDOCK_KF6_DECLARATIVE_LINK KF6::KDeclarative)
else()
    # Some distro packages export a different imported target name.
    get_property(_syndock_all_targets GLOBAL PROPERTY TARGETS)
    foreach(_syndock_target IN LISTS _syndock_all_targets)
        if(_syndock_target MATCHES "^KF6::.*Declarative.*$")
            set(SYNDOCK_KF6_DECLARATIVE_LINK ${_syndock_target})
            break()
        endif()
    endforeach()
endif()

if("${SYNDOCK_KF6_DECLARATIVE_LINK}" STREQUAL "" AND DEFINED KF6Declarative_LIBRARIES AND NOT "${KF6Declarative_LIBRARIES}" STREQUAL "")
    set(SYNDOCK_KF6_DECLARATIVE_LINK ${KF6Declarative_LIBRARIES})
    if(DEFINED KF6Declarative_INCLUDE_DIRS)
        set(SYNDOCK_KF6_DECLARATIVE_INCLUDE_DIRS ${KF6Declarative_INCLUDE_DIRS})
    endif()
endif()

if("${SYNDOCK_KF6_DECLARATIVE_LINK}" STREQUAL "")
    # Last-resort manual discovery for environments with incomplete CMake metadata.
    find_path(SYNDOCK_KF6_DECLARATIVE_INCLUDE_DIR
        NAMES
            KDeclarative/KDeclarative
            KDeclarative/ConfigPropertyMap
        PATH_SUFFIXES
            KF6
            include
            include/KF6
    )
    find_library(SYNDOCK_KF6_DECLARATIVE_LIBRARY
        NAMES
            KF6Declarative
            KDeclarative
            kdeclarative
    )

    if(SYNDOCK_KF6_DECLARATIVE_INCLUDE_DIR AND SYNDOCK_KF6_DECLARATIVE_LIBRARY)
        set(SYNDOCK_KF6_DECLARATIVE_LINK ${SYNDOCK_KF6_DECLARATIVE_LIBRARY})
        set(SYNDOCK_KF6_DECLARATIVE_INCLUDE_DIRS ${SYNDOCK_KF6_DECLARATIVE_INCLUDE_DIR})
    endif()
endif()

if("${SYNDOCK_KF6_DECLARATIVE_LINK}" STREQUAL "")
    message(STATUS "KF6Declarative_FOUND=${KF6Declarative_FOUND}")
    message(STATUS "KF6Declarative_LIBRARIES=${KF6Declarative_LIBRARIES}")
    message(STATUS "KF6Declarative_INCLUDE_DIRS=${KF6Declarative_INCLUDE_DIRS}")
    message(STATUS "SYNDOCK_KF6_DECLARATIVE_INCLUDE_DIR=${SYNDOCK_KF6_DECLARATIVE_INCLUDE_DIR}")
    message(STATUS "SYNDOCK_KF6_DECLARATIVE_LIBRARY=${SYNDOCK_KF6_DECLARATIVE_LIBRARY}")
    message(FATAL_ERROR "KF6 Declarative linkage not found. Install libkf6declarative-dev (or distro equivalent).")
endif()

# Share resolved declarative linkage with subdirectories.
set(SYNDOCK_KF6_DECLARATIVE_LINK "${SYNDOCK_KF6_DECLARATIVE_LINK}" CACHE INTERNAL "Resolved KF6 Declarative linkage")
set(SYNDOCK_KF6_DECLARATIVE_INCLUDE_DIRS "${SYNDOCK_KF6_DECLARATIVE_INCLUDE_DIRS}" CACHE INTERNAL "Resolved KF6 Declarative include dirs")

# ============================================================================
# Plasma 6 - Deep integration with KDE Plasma
# ============================================================================
find_package(Plasma ${PLASMA_MIN_VERSION} REQUIRED)
find_package(PlasmaQuick ${PLASMA_MIN_VERSION} REQUIRED)
find_package(PlasmaActivities REQUIRED)  # Activities moved from KF5 to Plasma in KF6
find_package(LibTaskManager REQUIRED)
add_definitions(-DPLASMA_WORKSPACE_VERSION="${LibTaskManager_VERSION}")

# ============================================================================
# Wayland - LayerShell for panel surfaces (KWin 6.x)
# ============================================================================
find_package(LayerShellQt REQUIRED)
find_package(PlasmaWaylandProtocols 1.6 REQUIRED)
find_package(Wayland REQUIRED COMPONENTS Client)
find_package(KWayland QUIET)
find_package(KF6WaylandClient QUIET)
find_package(PkgConfig QUIET)

# Wayland-only build - explicitly disable X11
set(HAVE_X11 OFF)
message(STATUS "SynDock: Wayland-only build (X11 support removed)")

# KF6 version info
message(STATUS "Qt6 VERSION          : ${Qt6_VERSION}")
message(STATUS "KF6 VERSION          : ${KF6_VERSION}")
message(STATUS "Plasma VERSION       : ${Plasma_VERSION}")
message(STATUS "LayerShellQt         : Found")

# ============================================================================
# QML Module verification
# ============================================================================
include(ECMFindQmlModule)
ecm_find_qmlmodule(QtQuick 6.0)
ecm_find_qmlmodule(QtQuick.Layouts 6.0)
ecm_find_qmlmodule(QtQuick.Controls 6.0)
ecm_find_qmlmodule(org.kde.plasma.core 6.0)
ecm_find_qmlmodule(org.kde.plasma.components 6.0)
ecm_find_qmlmodule(org.kde.kirigami 6.0)

# ============================================================================
# Configuration header
# ============================================================================
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/app/config-syndock.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/app/config-syndock.h)

# Compiler flags - enable warnings for quality code
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat -Werror=format-security")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")

message(STATUS "COMPILER FLAGS       : ${CMAKE_CXX_FLAGS}")

# ============================================================================
# Share Core Types between modules
# ============================================================================
set(CORETYPESHEADER "LIBCORETYPES_H")
configure_file(declarativeimports/coretypes.h.in declarativeimports/core/types.h)
set(CORETYPESHEADER "APPCORETYPES_H")
configure_file(declarativeimports/coretypes.h.in app/coretypes.h)
set(CORETYPESHEADER "NSECONTAINMENTTYPES_H")
configure_file(declarativeimports/coretypes.h.in containment/plugin/nsetypes.h)

# Share settings tools to containment actions
configure_file(app/settings/generic/generictools.h containmentactions/contextmenu/generictools.h)
configure_file(app/settings/generic/generictools.cpp containmentactions/contextmenu/generictools.cpp)
configure_file(app/data/contextmenudata.h containmentactions/contextmenu/contextmenudata.h)

# ============================================================================
# Subdirectories
# ============================================================================
add_subdirectory(declarativeimports)
add_subdirectory(indicators)
add_subdirectory(app)
add_subdirectory(containmentactions)
add_subdirectory(containment)
add_subdirectory(icons)
add_subdirectory(plasmoid)
add_subdirectory(shell)

ki18n_install(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
