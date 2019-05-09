set(units_Headers
  unit_definitions.hpp
  units_decl.hpp
  units.hpp
)

list(TRANSFORM units_Headers PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

# TODO: this is supposed to be public. figure out relative/absolute pathing...
target_sources(units PRIVATE ${units_Headers})

install(FILES ${units_Headers} DESTINATION include/units)
