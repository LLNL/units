# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2019,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
if (CMAKE_VERSION VERSION_GREATER 3.12)

option(UNITS_BUILD_WEBSERVER "Build a unit webserver for unit string conversions requires boost 1.70 or greater" OFF)

if (UNITS_BUILD_WEBSERVER)

find_package(Boost 1.70.0 REQUIRED)
add_executable(unit_web_server unit_web_server.cpp)
  target_link_libraries(unit_web_server units::units compile_flags_target Boost::boost)
  target_compile_definitions(unit_web_server PUBLIC BOOST_DATE_TIME_NO_LIB)
endif()
endif()