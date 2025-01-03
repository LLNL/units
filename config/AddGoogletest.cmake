# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2017-2025, Battelle Memorial Institute; Lawrence Livermore
# National Security, LLC; Alliance for Sustainable Energy, LLC.
# See the top-level NOTICE for additional details.
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#
# Add make check, as well, which gives output on failed tests without having to set an
# environment variable.
#
include(extraMacros)
set(CMAKE_WARN_DEPRECATED
    OFF
    CACHE INTERNAL "" FORCE
)
set(gtest_force_shared_crt
    ON
    CACHE INTERNAL ""
)

set(BUILD_SHARED_LIBS
    OFF
    CACHE INTERNAL ""
)
set(HAVE_STD_REGEX
    ON
    CACHE INTERNAL ""
)

set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS
    1
    CACHE INTERNAL ""
)

add_subdirectory(
    ${CMAKE_SOURCE_DIR}/ThirdParty/googletest ${CMAKE_BINARY_DIR}/ThirdParty/googletest
    EXCLUDE_FROM_ALL
)

if(NOT MSVC)
    target_compile_options(gtest PUBLIC "-Wno-undef" "-Wno-c++17-attribute-extensions")
    target_compile_options(gmock PUBLIC "-Wno-undef" "-Wno-c++17-attribute-extensions")
    target_compile_options(
        gtest_main PUBLIC "-Wno-undef" "-Wno-c++17-attribute-extensions"
    )
    target_compile_options(
        gmock_main PUBLIC "-Wno-undef" "-Wno-c++17-attribute-extensions"
    )
endif()

if(GOOGLE_TEST_INDIVIDUAL)
    include(GoogleTest)
endif()

function(add_unit_test test_source_file)
    get_filename_component(test_name "${test_source_file}" NAME_WE)
    add_executable("${test_name}" "${test_source_file}")
    target_link_libraries("${test_name}" gtest gmock gtest_main)
    add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${test_name}>)
    set_target_properties(${test_name} PROPERTIES FOLDER "Tests")
endfunction()

# Target must already exist
macro(add_gtest TESTNAME)
    target_link_libraries(${TESTNAME} PUBLIC gtest gmock gtest_main)

    if(GOOGLE_TEST_INDIVIDUAL)
        gtest_discover_tests(
            ${TESTNAME}
            TEST_PREFIX "${TESTNAME}."
            PROPERTIES FOLDER "Tests"
        )
    else()
        add_test(${TESTNAME} ${TESTNAME})
        set_target_properties(${TESTNAME} PROPERTIES FOLDER "Tests")
    endif()

endmacro()

hide_variable(gmock_build_tests)
hide_variable(gtest_build_samples)
hide_variable(gtest_build_tests)
hide_variable(gtest_disable_pthreads)
hide_variable(gtest_hide_internal_symbols)
hide_variable(BUILD_GMOCK)
hide_variable(BUILD_GTEST)
hide_variable(INSTALL_GTEST)
hide_variable(GTEST_HAS_ABSL)

set_target_properties(gtest gtest_main gmock gmock_main PROPERTIES FOLDER "Extern")

if(MSVC)
    # add_compile_options( /wd4459)
    if(MSVC_VERSION GREATER_EQUAL 1900)
        target_compile_definitions(
            gtest PUBLIC _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
        )
        target_compile_definitions(
            gtest_main PUBLIC _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
        )
        target_compile_definitions(
            gmock PUBLIC _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
        )
        target_compile_definitions(
            gmock_main PUBLIC _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
        )
    endif()
endif()
