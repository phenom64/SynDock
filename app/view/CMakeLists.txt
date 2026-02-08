# SynDock View Components
# Copyright (C) 2026 Syndromatic Ltd. All rights reserved
# SPDX-License-Identifier: GPL-2.0-or-later

set(syndock-app_SRCS
    ${syndock-app_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/clonedview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/containmentinterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/effects.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/eventssink.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/panelshadows.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parabolic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nseparaboliceffect.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nseparaboliceffect.h
    ${CMAKE_CURRENT_SOURCE_DIR}/positioner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasksmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/view.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/originalview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/visibilitymanager.cpp
    PARENT_SCOPE
)
