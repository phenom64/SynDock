# SynDock Declarative Imports
# Copyright (C) 2026 Syndromatic Ltd.

add_subdirectory(core)

# Install QML modules for SynDock abilities and components
install(DIRECTORY abilities/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/syndock/abilities)
install(DIRECTORY components/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/syndock/components)
