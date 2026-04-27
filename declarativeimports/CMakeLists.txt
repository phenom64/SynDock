add_subdirectory(core)
install(DIRECTORY abilities/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/syndromatic/syndock/abilities)
install(DIRECTORY components/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/syndromatic/syndock/components)
install(DIRECTORY effects/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/syndromatic/syndock/effects)

