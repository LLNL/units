cmake_minimum_required(VERSION 3.5)

project(UNITS LANGUAGES CXX VERSION 0.1.0)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # User settable
    set(UNITS_CXX_STD "11"  CACHE STRING "The CMake standard to require")

    set(UNITS_CUR_PROJ ON)
    set(CMAKE_CXX_STANDARD ${UNITS_CXX_STD})
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
	include(GNUInstallDirs)

else()
	set(UNITS_CUR_PROJ OFF)
endif()


list(APPEND CMAKE_MODULE_PATH "${UNITS_SOURCE_DIR}/config")

# Allow IDE's to group targets into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(
  UNITS_ENABLE_TESTS
  "Enable tests for the units library"
  ${UNITS_CUR_PROJ}
)

option(
  UNITS_HEADER_ONLY
  "Expose the units library as header-only"
  OFF
)

option(
  BUILD_UNITS_FUZZ_TARGETS
  "Build the targets for a fuzzing system"
  OFF
)

include(compiler_flags)

if (UNITS_HEADER_ONLY)
  target_compile_definitions(units PUBLIC UNITS_HEADER_ONLY)
endif(UNITS_HEADER_ONLY)


add_subdirectory(units)

if (BUILD_UNITS_FUZZ_TARGETS)
  add_subdirectory(FuzzTargets)
elseif (UNITS_ENABLE_TESTS)
  enable_testing()
  add_subdirectory(test)
endif ()

if (NOT UNITS_INSTALL_EXPORT)
	set(UNITS_INSTALL_EXPORT unitsConfig)
endif()

install(EXPORT ${UNITS_INSTALL_EXPORT}
  NAMESPACE units::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/units
)

export(EXPORT ${UNITS_INSTALL_EXPORT})

