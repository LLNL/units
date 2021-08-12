# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2017-2020, Battelle Memorial Institute; Lawrence Livermore
# National Security, LLC; Alliance for Sustainable Energy, LLC.
# See the top-level NOTICE for additional details.
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

cmake_minimum_required(VERSION 3.10...3.17)

project(units-find-package-test)

include(CTest)

# Test the HELICS CMake package config
find_package(UNITS 0.6 REQUIRED)

# add some tests inside of the CMake

message(STATUS "Binary location is ${PROJECT_BINARY_DIR}")
# Test the CXX shared library target
if(UNITS_BUILD_SHARED_LIBRARY)
    add_executable(shared-library-test-exe ../pkg_test_code/main.cpp)
    target_link_libraries(shared-library-test-exe UNITS::units)

    add_test(NAME shared-library-test COMMAND shared-library-test-exe)
    set_property(TEST shared-library-test PROPERTY PASS_REGULAR_EXPRESSION "${HELICS_VERSION}")

# Test the C and C++98 targets
elseif(UNITS_HEADER_ONLY)
    add_executable(header_only-exe ../pkg_test_code/header_only.cpp)
    target_link_libraries(header_only-exe UNITS::units)

    add_test(NAME header_only-exe COMMAND header_only-exe)
    set_property(TEST c-shared-library-test PROPERTY PASS_REGULAR_EXPRESSION "${HELICS_VERSION}")

endif()
