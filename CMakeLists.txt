cmake_minimum_required(VERSION 3.5)

project(UNITS VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_MODULE_PATH "${UNITS_SOURCE_DIR}/config")

# Allow IDE's to group targets into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(
  UNITS_ENABLE_TESTS
  "Enable tests for the units library"
  ON
)

option(
  UNITS_HEADER_ONLY
  "Expose the units library as header-only"
  OFF
)

option(
  BUILD_FUZZ_TARGETS
  "Build the targets for a fuzzing system"
  OFF
)

#include(compiler_flags)

if (UNITS_HEADER_ONLY)
  target_compile_definitions(units PUBLIC UNITS_HEADER_ONLY)
endif(UNITS_HEADER_ONLY)


add_subdirectory(units)

if (BUILD_FUZZ_TARGETS)
  add_subdirectory(FuzzTargets)
elseif (UNITS_ENABLE_TESTS)
  enable_testing()
  add_subdirectory(test)
endif ()



install(TARGETS unitslib
  EXPORT unitsConfig
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  INCLUDES DESTINATION include/units
)

install(EXPORT unitsConfig
  NAMESPACE units::
  DESTINATION lib/cmake/units
)

export(EXPORT unitsConfig)

