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

if (UNITS_HEADER_ONLY)
  # TODO:
  # install units_Sources
  # add this directory to the include path
else (UNITS_HEADER_ONLY)
  add_library(unitslib STATIC ${units_source_files} ${units_header_files})
  target_include_directories(unitslib PUBLIC  $<BUILD_INTERFACE:${UNITS_SOURCE_DIR}>)
endif (UNITS_HEADER_ONLY)

#install(FILES ${units_headers_files} DESTINATION units)