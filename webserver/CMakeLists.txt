# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2019-2025,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
if(CMAKE_VERSION VERSION_GREATER 3.12)

    cmake_dependent_option(
        UNITS_BUILD_WEBSERVER
        "Build a unit webserver for unit string conversions, requires boost 1.70 or greater"
        OFF
        "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME"
        OFF
    )

    if(UNITS_BUILD_WEBSERVER)

        find_package(Boost 1.70.0 REQUIRED)
        add_executable(units_webserver unit_web_server.cpp)
        target_link_libraries(
            units_webserver PUBLIC ${UNITS_LC_PROJECT_NAME}::units compile_flags_target
                                   Boost::boost
        )

        target_compile_definitions(units_webserver PUBLIC BOOST_DATE_TIME_NO_LIB)
        target_compile_definitions(
            units_webserver PUBLIC UNITS_VERSION_STRING="${UNITS_VERSION}"
        )
        if(MSYS OR CYGWIN)
            target_link_libraries(units_webserver PUBLIC wsock32 ws2_32 iphlpapi)
        elseif(NOT WIN32)
            set(THREADS_PREFER_PTHREAD_FLAG ON)
            find_package(Threads REQUIRED)
            target_link_libraries(units_webserver PUBLIC Threads::Threads)
        endif()

        if(UNITS_INSTALL)
            install(
                TARGETS units_webserver ${UNITS_LIBRARY_EXPORT_COMMAND}
                DESTINATION "${CMAKE_INSTALL_BINDIR}"
                COMPONENT units_webserver
            )
        endif()
        add_executable(${UNITS_LC_PROJECT_NAME}::units_webserver ALIAS units_webserver)
    endif()
endif()
