include(ECMInstallIcons)

ecm_install_icons(ICONS
    16-apps-syndock.svg
    22-apps-syndock.svg
    24-apps-syndock.svg
    32-apps-syndock.svg
    48-apps-syndock.svg
    sc-apps-syndock.svg
THEME hicolor
DESTINATION ${KDE_INSTALL_ICONDIR})

install(FILES org.syndromatic.syndock.plasmoid.svg DESTINATION ${KDE_INSTALL_ICONDIR}/breeze/applets/256)
