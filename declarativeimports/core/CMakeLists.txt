configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-syndock-lib.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-syndock-lib.h)

include_directories(../../legacy)

set(syndockcoreplugin_SRCS
    syndockcoreplugin.cpp
    dialog.cpp
    environment.cpp
    ../../legacy/ManagedTextureNode.cpp
    iconitem.cpp
    quickwindowsystem.cpp
    tools.cpp
    types.h
)

#include_directories(${Plasma_INCLUDE_DIRS})
add_library(syndockcoreplugin SHARED ${syndockcoreplugin_SRCS})

target_link_libraries(syndockcoreplugin
    Qt::Quick
    Qt::Qml
    KF6::CoreAddons
    KF6::IconThemes
    KF6::Svg
    Plasma::Plasma
    Plasma::PlasmaQuick
)

target_link_libraries(syndockcoreplugin KF6::WindowSystem)

install(TARGETS syndockcoreplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/syndromatic/syndock/core)

install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/syndromatic/syndock/core)
