add_library(goby_frontseat_moos_gateway_plugin SHARED frontseat_gateway_plugin.cpp)
target_link_libraries(goby_frontseat_moos_gateway_plugin
  goby goby_zeromq goby_moos)

set_target_properties(goby_frontseat_moos_gateway_plugin PROPERTIES VERSION "${GOBY_VERSION}" SOVERSION "${GOBY_SOVERSION}")
goby_install_lib(goby_frontseat_moos_gateway_plugin moos)
