add_subdirectory(units)

target_include_directories(units PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
  $<INSTALL_INTERFACE:include>)
