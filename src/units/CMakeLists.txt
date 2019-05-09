set(units_Sources
  units.cpp
  x12_conv.cpp
  r20_conv.cpp
  commodities.cpp
)

if (UNITS_HEADER_ONLY)
  # TODO:
  # install units_Sources
  # add this directory to the include path
else (UNITS_HEADER_ONLY)
  list(TRANSFORM units_Sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
  target_sources(units PRIVATE ${units_Sources})
endif (UNITS_HEADER_ONLY)
