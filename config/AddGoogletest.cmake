# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2017-2019, Battelle Memorial Institute; Lawrence Livermore
# National Security, LLC; Alliance for Sustainable Energy, LLC.
# See the top-level NOTICE for additional details.
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# ~~~
# Copyright (c) 2020,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
# ~~~

#
# Downloads GTest and provides a helper macro to add tests. Add make check, as well,
# which gives output on failed tests without having to set an environment variable.
#
include(extraMacros)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 3.8)
        set(gtest_version f3ef7e1)
    else()
        set(gtest_version 703bd9c)
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
        set(gtest_version f3ef7e1)
    else()
        set(gtest_version 703bd9c)
    endif()
else()
    set(gtest_version f3ef7e1)
endif()

# depending on what the version is set to the git_clone command may need to change to
# GIT_TAG||GIT_BRANCH|GIT_COMMIT

string(TOLOWER "googletest" gtName)

if(NOT CMAKE_VERSION VERSION_LESS 3.11)
    include(FetchContent)
    mark_as_advanced(FETCHCONTENT_BASE_DIR)
    mark_as_advanced(FETCHCONTENT_FULLY_DISCONNECTED)
    mark_as_advanced(FETCHCONTENT_QUIET)

    fetchcontent_declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG ${gtest_version}
    )

    fetchcontent_getproperties(googletest)

    if(NOT ${gtName}_POPULATED)
        # Fetch the content using previously declared details
        fetchcontent_populate(googletest)

    endif()
    hide_variable(FETCHCONTENT_SOURCE_DIR_GOOGLETEST)
    hide_variable(FETCHCONTENT_UPDATES_DISCONNECTED_GOOGLETEST)
else() # cmake <3.11

    # create the directory first
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/_deps)

    include(GitUtils)
    git_clone(
        PROJECT_NAME
        googletest
        GIT_URL
        https://github.com/google/googletest.git
        GIT_BRANCH
        v1.10.x
        DIRECTORY
        ${PROJECT_BINARY_DIR}/_deps
    )

    set(${gtName}_BINARY_DIR ${PROJECT_BINARY_DIR}/_deps/${gtName}-build)

endif()

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
add_subdirectory(${${gtName}_SOURCE_DIR} ${${gtName}_BINARY_DIR} EXCLUDE_FROM_ALL)

message(STATUS "loading google-test directory ${${gtName}_SOURCE_DIR}")
if(NOT MSVC)
    # target_Compile_options(gtest PRIVATE "-Wno-undef") target_Compile_options(gmock
    # PRIVATE "-Wno-undef") target_Compile_options(gtest_main PRIVATE "-Wno-undef")
    # target_Compile_options(gmock_main PRIVATE "-Wno-undef")
endif()

if(GOOGLE_TEST_INDIVIDUAL)
    if(NOT CMAKE_VERSION VERSION_LESS 3.9)
        include(GoogleTest)
    else()
        set(GOOGLE_TEST_INDIVIDUAL OFF)
    endif()
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
        if(CMAKE_VERSION VERSION_LESS 3.10)
            gtest_add_tests(
                TARGET ${TESTNAME}
                TEST_PREFIX "${TESTNAME}."
                TEST_LIST TmpTestList
            )
            set_tests_properties(${TmpTestList} PROPERTIES FOLDER "Tests")
        else()
            gtest_discover_tests(
                ${TESTNAME}
                TEST_PREFIX "${TESTNAME}."
                PROPERTIES FOLDER "Tests"
            )

        endif()
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
