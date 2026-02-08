/* This file is a part of the Atmo desktop experience's SynDock project for SynOS .
 * Copyright (C) 2026 Syndromatic Ltd. All rights reserved
 * Designed by Kavish Krishnakumar in Manchester.
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Based on Latte Dock
 * SPDX-FileCopyrightText: 2016 Smith AR <audoban@openmailbox.org>
 * SPDX-FileCopyrightText: 2016 Michail Vourlakos <mvourlakos@gmail.com>
 */

#ifndef CONFIG_SYNDOCK_H
#define CONFIG_SYNDOCK_H

// Wayland-only build - X11 support removed
#define HAVE_X11 0

// KDE Frameworks 6 version
#cmakedefine KF6_VERSION_MINOR @KF6_VERSION_MINOR@

// SynDock version string
#cmakedefine VERSION "@syndock_VERSION@"

// Project metadata
#cmakedefine WEBSITE "@WEBSITE@"
#cmakedefine BUG_ADDRESS "@BUG_ADDRESS@"

// Plasma workspace version (from LibTaskManager)
#cmakedefine PLASMA_WORKSPACE_VERSION "@PLASMA_WORKSPACE_VERSION@"

#endif // CONFIG_SYNDOCK_H
