# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2019-2020,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

cmake_minimum_required(VERSION 3.0)

# Make sure users don't get warnings on a tested (3.0 to 3.16) version
# of CMake. For most of the policies, the new version is better (hence the change).
# We don't use the 3.0...3.16 syntax because of a bug in an older MSVC's
# built-in and modified CMake 3.11
if(${CMAKE_VERSION} VERSION_LESS 3.16)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.16)
endif()


project(
    UNITS
    LANGUAGES CXX
    VERSION 0.3.0
)
include(CMakeDependentOption)
include(CTest)
include(GNUInstallDirs)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND NOT DEFINED CMAKE_CXX_STANDARD)
    # User settable
    set(CMAKE_CXX_STANDARD 11)
endif()

if(NOT DEFINED CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if(NOT DEFINED CMAKE_CXX_STANDARD_REQUIRED)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()


list(APPEND CMAKE_MODULE_PATH "${UNITS_SOURCE_DIR}/config")

# Allow IDE's to group targets into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

cmake_dependent_option(
    UNITS_ENABLE_TESTS "Enable tests for the units library" ON
    "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" OFF
)

option(UNITS_INSTALL "Enable installation of the units library" ON)
mark_as_advanced(UNITS_INSTALL)

cmake_dependent_option(
    UNITS_WITH_CMAKE_PACKAGE
    "Generate and install cmake package files"
    ON
    "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME;UNITS_INSTALL;NOT UNITS_BINARY_ONLY_INSTALL"
    OFF
)

mark_as_advanced(UNITS_WITH_CMAKE_PACKAGE)

cmake_dependent_option(
    UNITS_BUILD_FUZZ_TARGETS "Build the targets for a fuzzing system" OFF
    "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" OFF
)

# Install instructions for this target
if(UNITS_WITH_CMAKE_PACKAGE)
    set(UNITS_LIBRARY_EXPORT_COMMAND EXPORT unitsConfig)
else(UNITS_WITH_CMAKE_PACKAGE)
    set(UNITS_LIBRARY_EXPORT_COMMAND)
endif(UNITS_WITH_CMAKE_PACKAGE)

option(UNITS_HEADER_ONLY "Expose the units library as header-only" OFF)

if(NOT TARGET compile_flags_target)
    add_library(compile_flags_target INTERFACE)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(compiler_flags)
endif()

if(NOT UNITS_HEADER_ONLY)
    if(BUILD_SHARED_LIBS)
        option(UNITS_BUILD_STATIC_LIBRARY
               "enable Construction of the units static library" OFF
        )
        option(UNITS_BUILD_SHARED_LIBRARY
               "enable Construction of the units shared library" ON
        )
    else(BUILD_SHARED_LIBS)
        option(UNITS_BUILD_STATIC_LIBRARY
               "enable Construction of the units static library" ON
        )
        option(UNITS_BUILD_SHARED_LIBRARY
               "enable Construction of the units shared library" OFF
        )
    endif(BUILD_SHARED_LIBS)

    cmake_dependent_option(
        UNITS_BUILD_OBJECT_LIBRARY "Enable construction of the units object library"
        OFF "NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" OFF
    )

endif(NOT UNITS_HEADER_ONLY)

add_subdirectory(units)

if(UNITS_BUILD_FUZZ_TARGETS)
    add_subdirectory(FuzzTargets)
elseif(UNITS_ENABLE_TESTS)
    enable_testing()
    if(BUILD_TESTING)
        add_subdirectory(test)
    endif()
endif()

add_subdirectory(webserver)

if(UNITS_INSTALL)
    if(UNITS_BUILD_STATIC_LIBRARY)
        install(TARGETS compile_flags_target ${UNITS_LIBRARY_EXPORT_COMMAND})
    endif()
    if(UNITS_WITH_CMAKE_PACKAGE AND NOT UNITS_BINARY_ONLY_INSTALL)
        install(EXPORT unitsConfig DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/units)
        export(EXPORT unitsConfig)
    endif()
endif()
