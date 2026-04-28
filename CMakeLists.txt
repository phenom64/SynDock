cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
project(syndock VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(VERSION 1.0.0)
set(AUTHOR "Syndromatic Ltd.")
set(EMAIL "kavish@syndromatic.com")
set(WEBSITE "https://syndromatic.com/syndock")
set(BUG_ADDRESS "https://github.com/phenom64/SynDock/issues")

if(DEFINED ENV{GENERATE_COMPILE_COMMANDS})
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

set(QT_MIN_VERSION "6.10.2")
set(KF6_MIN_VERSION "6.24.0")
set(PLASMA_MIN_VERSION "6.6.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.84.0")

set(KF6_LOCALE_PREFIX "")

find_package(ECM ${KF6_MIN_VERSION} REQUIRED )
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs6)
include(CheckIncludeFiles)
include(CMakePackageConfigHelpers)
include(ECMOptionalAddSubdirectory)
include(ECMQtDeclareLoggingCategory)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEPackageAppTemplates)
include(WriteBasicConfigVersionFile)


find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED NO_MODULE COMPONENTS DBus Gui Qml Quick WaylandClient)

#find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
#    Activities Archive CoreAddons GuiAddons Crash DBusAddons Declarative GlobalAccel Kirigami2
#    I18n IconThemes KIO NewStuff Notifications Plasma PlasmaQuick Wayland WindowSystem XmlGui)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    Archive
    CoreAddons
    Crash
    DBusAddons
    Declarative
    GlobalAccel
    GuiAddons
    I18n
    IconThemes
    KIO
    Kirigami
    NewStuff
    Notifications
    Package
    Svg
    WindowSystem
    XmlGui
    Config
)

find_package(Plasma ${PLASMA_MIN_VERSION} REQUIRED)
find_package(PlasmaActivities ${PLASMA_MIN_VERSION} REQUIRED)
find_package(PlasmaQuick ${PLASMA_MIN_VERSION} REQUIRED)
find_package(KWayland ${PLASMA_MIN_VERSION} REQUIRED)
find_package(LayerShellQt REQUIRED)

find_package(LibTaskManager REQUIRED)
add_definitions(-DPLASMA_WORKSPACE_VERSION="${LibTaskManager_VERSION}")

set(SYNDOCK_WAYLAND_ONLY ON)
message(STATUS "SynDock: Wayland-only build; X11 support is disabled")


find_package(QtWaylandScanner REQUIRED)
find_package(Qt${QT_MAJOR_VERSION}WaylandClient)
find_package(PlasmaWaylandProtocols 1.6 REQUIRED)
find_package(Wayland REQUIRED COMPONENTS Client)

string(REGEX MATCH "\\.([^]]+)\\." KF6_VERSION_MINOR ${KF6_VERSION})
string(REGEX REPLACE "\\." "" KF6_VERSION_MINOR ${KF6_VERSION_MINOR})

message(STATUS "KF6 VERSION : ${KF6_VERSION}")
message(STATUS "KF6 VERSION MINOR : ${KF6_VERSION_MINOR}")


include(ECMFindQmlModule)
ecm_find_qmlmodule(org.kde.kquickcontrolsaddons 2.0 REQUIRED)
ecm_find_qmlmodule(org.kde.plasma.core 2.0 REQUIRED)
ecm_find_qmlmodule(org.kde.plasma.components 2.0 REQUIRED)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/app/config-syndock.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/app/config-syndock.h)

#hide warnings
string(REPLACE "-Wall" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
string(REPLACE "-Wdeprecated-declarations" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
string(REPLACE "-Wreorder" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
string(REPLACE "-Wunused-variable" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
string(REPLACE "-Wunused-parameter" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

#add format security check
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat -Werror=format-security")

message(STATUS "COMPILER FLAGS : ${CMAKE_CXX_FLAGS}")

# Share Same Core Types between declarativeimports and app
set(CORETYPESHEADER "LIBCORETYPES_H")
configure_file(declarativeimports/coretypes.h.in declarativeimports/core/types.h)
set(CORETYPESHEADER "APPCORETYPES_H")
configure_file(declarativeimports/coretypes.h.in app/coretypes.h)
set(CORETYPESHEADER "NSEGENERALCONTAINMENTTYPES_H")
configure_file(declarativeimports/coretypes.h.in containment/plugin/nsetypes.h)

# Share App::Settings::GenericTools to containment actions
configure_file(app/settings/generic/generictools.h containmentactions/contextmenu/generictools.h)
configure_file(app/settings/generic/generictools.cpp containmentactions/contextmenu/generictools.cpp)
configure_file(app/data/contextmenudata.h containmentactions/contextmenu/contextmenudata.h)

# subdirectories
add_subdirectory(declarativeimports)
add_subdirectory(indicators)
add_subdirectory(app)
add_subdirectory(containmentactions)
add_subdirectory(containment)
add_subdirectory(icons)
add_subdirectory(plasmoid)
add_subdirectory(shell)

# Translation catalogs still use Latte domains and must be regenerated after
# the SynDock message-domain pass.
# ki18n_install(po)
