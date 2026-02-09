# SynDock indicator package plugin
kcoreaddons_add_plugin(syndock_packagestructure_indicator SOURCES indicatorpackage.cpp INSTALL_NAMESPACE kpackage/packagestructure)
set_target_properties(syndock_packagestructure_indicator PROPERTIES OUTPUT_NAME syndock_indicator)

target_link_libraries(syndock_packagestructure_indicator
   ${SYNDOCK_KF6_DECLARATIVE_LINK}
   KF6::I18n
   KF6::Package
)

if(NOT "${SYNDOCK_KF6_DECLARATIVE_INCLUDE_DIRS}" STREQUAL "")
    target_include_directories(syndock_packagestructure_indicator PRIVATE ${SYNDOCK_KF6_DECLARATIVE_INCLUDE_DIRS})
endif()

# Service type files moved to kpackage metadata in KF6 - desktop file not needed
