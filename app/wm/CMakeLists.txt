# SynDock Window Manager Integration
# Copyright (C) 2026 Syndromatic Ltd. All rights reserved
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Wayland-only build - X11 interface removed

set(syndock-app_SRCS
    ${syndock-app_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/abstractwindowinterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/schemecolors.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nsewaylandinterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nsewaylandinterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/windowinfowrap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasktools.cpp
    # NOTE: xwindowinterface.cpp removed - Wayland-only build
    PARENT_SCOPE
)
