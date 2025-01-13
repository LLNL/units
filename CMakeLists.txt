# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2019-2025,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# Make sure users don't get warnings on a tested (3.0 to 3.31) version of CMake. For
# most of the policies, the new version is better (hence the change). We don't use the
# 3.0...3.17 syntax because of a bug in an older MSVC's built-in and modified CMake 3.11
if(${CMAKE_VERSION} VERSION_GREATER 3.22)
    cmake_minimum_required(VERSION 3.22...3.31)
else()
    cmake_minimum_required(VERSION 3.0)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

if(NOT UNITS_CMAKE_PROJECT_NAME)
    set(UNITS_CMAKE_PROJECT_NAME UNITS)

endif()

project(
    ${UNITS_CMAKE_PROJECT_NAME}
    LANGUAGES C CXX
    VERSION 0.12.1
)
include(CMakeDependentOption)
include(CTest)
include(GNUInstallDirs)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND NOT DEFINED CMAKE_CXX_STANDARD)
    # User settable
    set(CMAKE_CXX_STANDARD 14)
endif()

if(NOT DEFINED CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if(NOT DEFINED CMAKE_CXX_STANDARD_REQUIRED)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# Set the build output paths
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
            "${CMAKE_BINARY_DIR}/lib"
            CACHE PATH "Archive output dir."
        )
    endif()
    if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
            "${CMAKE_BINARY_DIR}/lib"
            CACHE PATH "Library output dir."
        )
    endif()
    if(NOT CMAKE_PDB_OUTPUT_DIRECTORY)
        set(CMAKE_PDB_OUTPUT_DIRECTORY
            "${CMAKE_BINARY_DIR}/bin"
            CACHE PATH "PDB (MSVC debug symbol)output dir."
        )
    endif()
    if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
            "${CMAKE_BINARY_DIR}/bin"
            CACHE PATH "Executable/dll output dir."
        )
    endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/config")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/cmake")

# Allow IDE's to group targets into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

cmake_dependent_option(
    UNITS_ENABLE_TESTS "Enable tests for the units library" ON
    "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" OFF
)

# allow projects that include cmake to set their own namespace for the main library
set(UNITS_NAMESPACE
    ""
    CACHE STRING "Top-level namespace name. Default is `units`."
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND NOT UNITS_BINARY_ONLY_INSTALL)
    option(UNITS_INSTALL
           "Generate and install cmake package files and shared library if built" ON
    )
else()
    option(UNITS_INSTALL
           "Generate and install cmake package files and shared library if built" OFF
    )
endif()

mark_as_advanced(UNITS_INSTALL)

cmake_dependent_option(
    UNITS_BUILD_FUZZ_TARGETS "Build the targets for a fuzzing system" OFF
    "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" OFF
)

cmake_dependent_option(
    UNITS_CLANG_TIDY "Look for and use Clang-Tidy" OFF
    "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME;NOT CMAKE_VERSION VERSION_LESS 3.6" OFF
)

cmake_dependent_option(
    UNITS_BUILD_PYTHON_LIBRARY "Build the simplified python library" OFF
    "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" OFF
)
set(UNITS_CLANG_TIDY_OPTIONS
    ""
    CACHE STRING "Clang tidy options, such as -fix, semicolon separated"
)

mark_as_advanced(UNITS_CLANG_TIDY_OPTIONS)
mark_as_advanced(UNITS_CLANG_TIDY)

option(UNITS_HEADER_ONLY "Expose the units library as header-only" OFF)

if(NOT TARGET compile_flags_target)
    add_library(compile_flags_target INTERFACE)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(compiler_flags)
endif()

# deal with breaking changes in C++20 with u8 strings
if(CMAKE_CXX_STANDARD GREATER 19 OR UNITS_CXX_STANDARD GREATER 19)
    if(MSVC)
        target_compile_options(compile_flags_target INTERFACE /Zc:char8_t-)
    else(MSVC)
        target_compile_options(compile_flags_target INTERFACE -fno-char8_t)
    endif()
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

else()
    option(UNITS_BUILD_STATIC_LIBRARY "enable Construction of the units static library"
           OFF
    )
    option(UNITS_BUILD_SHARED_LIBRARY "enable Construction of the units shared library"
           OFF
    )
endif(NOT UNITS_HEADER_ONLY)

if(UNITS_BUILD_SHARED_LIBRARY AND UNITS_BUILD_STATIC_LIBRARY)
    message(
        WARNING
            "Both UNITS_BUILD_SHARED_LIBRARY and UNITS_BUILD_STATIC_LIBRARY are set to ON, only the shared library will be built"
    )
endif()

cmake_dependent_option(
    UNITS_BUILD_OBJECT_LIBRARY "Enable construction of the units object library" OFF
    "NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" OFF
)

if(UNITS_BUILD_SHARED_LIBRARY AND UNITS_BUILD_OBJECT_LIBRARY)
    message(
        WARNING
            "Both UNITS_BUILD_SHARED_LIBRARY and UNITS_BUILD_OBJECT_LIBRARY are set to ON, only the shared library will be built"
    )
elseif(UNITS_BUILD_STATIC_LIBRARY AND UNITS_BUILD_OBJECT_LIBRARY)
    message(
        WARNING
            "Both UNITS_BUILD_STATIC_LIBRARY and UNITS_BUILD_OBJECT_LIBRARY are set to ON, only the object library will be built"
    )
endif()

# Prepare Clang-Tidy
if(UNITS_CLANG_TIDY)
    find_program(
        CLANG_TIDY_EXE
        NAMES "clang-tidy"
        DOC "Path to clang-tidy executable" REQUIRED
    )

    set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" ${UNITS_CLANG_TIDY_OPTIONS})
endif()

string(TOLOWER ${UNITS_CMAKE_PROJECT_NAME} UNITS_LC_PROJECT_NAME)

set(UNITS_LIBRARY_EXPORT_COMMAND EXPORT unitsTargets)
mark_as_advanced(UNITS_LIBRARY_EXPORT_COMMAND)

add_subdirectory(units)

if(UNITS_BUILD_FUZZ_TARGETS)
    add_subdirectory(FuzzTargets)
elseif(UNITS_ENABLE_TESTS AND NOT CMAKE_VERSION VERSION_LESS 3.13)
    include(updateGitSubmodules)
    enable_testing()
    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/ThirdParty/googletest/CMakeLists.txt")
        submod_update(ThirdParty/googletest)
    endif()
    if(BUILD_TESTING)
        add_subdirectory(test)
    endif()

elseif(UNITS_ENABLE_TESTS)
    message(WARNING "UNITS unit tests only supported under cmake 3.13 or greater")
endif()

if(NOT UNITS_HEADER_ONLY
   AND NOT UNITS_BUILD_FUZZ_TARGETS
   AND NOT SKBUILD
)
    add_subdirectory(webserver)
    add_subdirectory(converter)
endif()

if(NOT UNITS_HEADER_ONLY AND UNITS_BUILD_PYTHON_LIBRARY)
    message(STATUS "building the python library")

    find_package(
        Python 3.10 REQUIRED
        COMPONENTS Interpreter Development.Module
        OPTIONAL_COMPONENTS Development.SABIModule
    )

    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_BUILD_TYPE
            Release
            CACHE STRING "Choose the type of build." FORCE
        )
        set_property(
            CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel"
                                            "RelWithDebInfo"
        )
    endif()

    # Detect the installed nanobind package and import it into CMake
    execute_process(
        COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE nanobind_ROOT
    )
    find_package(nanobind CONFIG REQUIRED)

    nanobind_add_module(units_llnl_ext NB_STATIC STABLE_ABI python/units_python.cpp)

    target_link_libraries(units_llnl_ext PRIVATE units::units)

    # Install directive for scikit-build-core
    install(TARGETS units_llnl_ext LIBRARY DESTINATION units_llnl)

endif()

if(UNITS_INSTALL AND NOT SKBUILD)
    if(UNITS_BUILD_STATIC_LIBRARY)
        install(TARGETS compile_flags_target ${UNITS_LIBRARY_EXPORT_COMMAND})
    endif()
    if(NOT UNITS_BINARY_ONLY_INSTALL)
        include(CMakePackageConfigHelpers)
        configure_file(
            config/unitsConfig.cmake.in
            "${PROJECT_BINARY_DIR}/${UNITS_LC_PROJECT_NAME}Config.cmake" @ONLY
        )

        export(
            EXPORT unitsTargets
            NAMESPACE ${UNITS_LC_PROJECT_NAME}::
            FILE "${PROJECT_BINARY_DIR}/${UNITS_LC_PROJECT_NAME}Targets.cmake"
        )

        install(
            EXPORT unitsTargets
            NAMESPACE ${UNITS_LC_PROJECT_NAME}::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${UNITS_LC_PROJECT_NAME}
        )

        write_basic_package_version_file(
            ${PROJECT_BINARY_DIR}/${UNITS_LC_PROJECT_NAME}ConfigVersion.cmake
            COMPATIBILITY AnyNewerVersion
        )
        install(FILES ${PROJECT_BINARY_DIR}/${UNITS_LC_PROJECT_NAME}ConfigVersion.cmake
                      ${PROJECT_BINARY_DIR}/${UNITS_LC_PROJECT_NAME}Config.cmake
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${UNITS_LC_PROJECT_NAME}
        )
    endif()
endif()
