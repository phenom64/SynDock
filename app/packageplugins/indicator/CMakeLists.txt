kcoreaddons_add_plugin(syndock_packagestructure_indicator SOURCES indicatorpackage.cpp INSTALL_NAMESPACE kf6/packagestructure)
set_target_properties(syndock_packagestructure_indicator PROPERTIES OUTPUT_NAME syndock_indicator)

target_link_libraries(syndock_packagestructure_indicator
   KF6::I18n
   KF6::Package
)
