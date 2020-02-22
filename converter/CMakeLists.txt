# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2019-2020,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    cmake_dependent_option(
        UNITS_BUILD_CONVERTER_APP
        "Build a unit converter app"
        ON
        "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME"
        OFF
    )

    if(UNITS_BUILD_CONVERTER_APP)
        add_executable(unit_convert converter.cpp)
        target_link_libraries(
            unit_convert PUBLIC units::units compile_flags_target
        )
    endif()

