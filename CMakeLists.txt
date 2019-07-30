cmake_minimum_required(VERSION 3.0)

project(UNITS LANGUAGES CXX VERSION 0.1.0)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # User settable
    set(UNITS_CXX_STD "11"  CACHE STRING "The CMake standard to require")

    set(UNITS_ROOT_PROJECT ON)
    set(CMAKE_CXX_STANDARD ${UNITS_CXX_STD})
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
	include(GNUInstallDirs)

else()
	set(UNITS_ROOT_PROJECT OFF)
endif()


list(APPEND CMAKE_MODULE_PATH "${UNITS_SOURCE_DIR}/config")

# Allow IDE's to group targets into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(
  UNITS_ENABLE_TESTS
  "Enable tests for the units library"
  ${UNITS_ROOT_PROJECT}
)

option(UNITS_WITH_CMAKE_PACKAGE "Generate and install cmake package files" ${UNITS_ROOT_PROJECT})
mark_as_advanced(UNITS_WITH_CMAKE_PACKAGE)


# Install instructions for this target
if(UNITS_WITH_CMAKE_PACKAGE)
    set(UNITS_LIBRARY_EXPORT_COMMAND EXPORT unitsConfig)
else(UNITS_WITH_CMAKE_PACKAGE)
    set(UNITS_LIBRARY_EXPORT_COMMAND)
endif(UNITS_WITH_CMAKE_PACKAGE)

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
else(UNITS_HEADER_ONLY)
	if (BUILD_SHARED_LIBS)
		option(BUILD_UNITS_STATIC_LIBRARY "enable Construction of the units static library" OFF)
		option(BUILD_UNITS_SHARED_LIBRARY "enable Construction of the units shared library" ON)
	else(BUILD_SHARED_LIBS)
		option(BUILD_UNITS_STATIC_LIBRARY "enable Construction of the units static library" ON)
		option(BUILD_UNITS_SHARED_LIBRARY "enable Construction of the units shared library" OFF)
	endif(BUILD_SHARED_LIBS)

	if (UNITS_ROOT_PROJECT)
		set(BUILD_UNITS_OBJECT_LIB OFF)
	else(UNITS_ROOT_PROJECT)
		option(UNITS_OBJECT_LIB "Enable construction of the units object library" OFF)
	endif(UNITS_ROOT_PROJECT)
endif(UNITS_HEADER_ONLY)


add_subdirectory(units)

if (BUILD_UNITS_FUZZ_TARGETS)
  add_subdirectory(FuzzTargets)
elseif (UNITS_ENABLE_TESTS)
	enable_testing()
	if (BUILD_TESTING)
	   add_subdirectory(tests)
	endif()
endif()

if(UNITS_WITH_CMAKE_PACKAGE)
        install(EXPORT unitsConfig
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/units
                )
		export(EXPORT unitsConfig)
endif()




