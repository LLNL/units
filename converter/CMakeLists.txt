# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2019-2021,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

cmake_dependent_option(
    UNITS_BUILD_CONVERTER_APP "Build a unit converter app" ON
    "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" OFF
)

if(UNITS_BUILD_CONVERTER_APP)
    add_executable(unit_convert converter.cpp)
    target_link_libraries(unit_convert PUBLIC units::units compile_flags_target)
    target_include_directories(unit_convert PRIVATE ${PROJECT_SOURCE_DIR}/ThirdParty)
    target_compile_definitions(
        unit_convert PUBLIC UNITS_VERSION_STRING="${UNITS_VERSION}"
    )
    if(CMAKE_CXX_STANDARD GREATER 16)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_VERSION
                                                    VERSION_EQUAL 8
        )
            target_link_libraries(unit_convert PRIVATE -lstdc++fs)
        endif()
    endif()
endif()
