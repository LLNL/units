set(units_source_files
  units.cpp
  x12_conv.cpp
  r20_conv.cpp
  commodities.cpp
)

set(units_header_files
  units.hpp
  units_decl.hpp
  unit_definitions.hpp
)

if (NOT DEFINED UNITS_LIBRARY_TYPE)
   set (UNITS_LIBRARY_TYPE STATIC CACHE STRING "set the units library type" )
   set_property(CACHE UNITS_LIBRARY_TYPE PROPERTY STRINGS STATIC SHARED OBJECT)
endif()

if (UNITS_HEADER_ONLY)
  # TODO:
  # install units_Sources
  # add this directory to the include path
else (UNITS_HEADER_ONLY)
  add_library(unitslib ${UNITS_LIBRARY_TYPE} ${units_source_files} ${units_header_files})
  target_include_directories(unitslib PUBLIC  $<BUILD_INTERFACE:${UNITS_SOURCE_DIR}>)
endif (UNITS_HEADER_ONLY)

add_library(units::units ALIAS unitslib)

set (UNITS_INSTALL_EXPORT unitsConfig)

if (NOT UNITS_LIBRARY_TYPE STREQUAL "OBJECT")
install(TARGETS unitslib
  EXPORT ${UNITS_INSTALL_EXPORT}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
 # INCLUDES DESTINATION include/units
)
install (FILES ${units_header_files} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()