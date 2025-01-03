# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2019-2025,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

include(AddGoogletest)

set(UNIT_TEST_HEADER_ONLY test_conversions1 test_equation_units test_measurement
                          test_pu test_unit_ops test_uncertain_measurements
)

set(UNITS_TESTS
    ${UNIT_TEST_HEADER_ONLY}
    fuzz_issue_tests
    test_conversions2
    test_random_round_trip
    test_ucum
    test_udunits
    test_unit_strings
    test_measurement_strings
    test_commodities
    test_siunits
    test_defined_units
    test_math
    test_google_units
    test_complete_unit_list
)

if(NOT UNITS_DISABLE_EXTRA_UNIT_STANDARDS)
    list(APPEND UNITS_TESTS test_r20)
endif()

set(TEST_FILE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/files)

# /wd4459 is for a warning of a global m in google test. They won't interfere so ignore
# it
if(UNITS_HEADER_ONLY)

    foreach(T ${UNIT_TEST_HEADER_ONLY})

        add_unit_test(${T}.cpp)
        target_link_libraries(
            ${T} ${UNITS_LC_PROJECT_NAME}::header_only compile_flags_target
        )
        if(MSVC)
            target_compile_options(${T} PRIVATE /wd4459)
        endif()
    endforeach()

    if(MSVC)
        target_compile_options(test_unit_ops PRIVATE /wd4127 /wd4459)
    endif()
else()
    foreach(T ${UNITS_TESTS})

        add_unit_test(${T}.cpp)
        target_link_libraries(${T} ${UNITS_LC_PROJECT_NAME}::units compile_flags_target)
        if(UNITS_CLANG_TIDY)
            set_property(TARGET ${T} PROPERTY CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
        endif()
        if(MSVC)
            target_compile_options(${T} PRIVATE /wd4459)
        endif()
    endforeach()

    target_compile_definitions(
        test_unit_strings PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}"
    )

    if(NOT UNITS_BUILD_SHARED_LIBRARY)
        target_compile_definitions(
            test_unit_strings PUBLIC -DENABLE_UNIT_TESTING=1 -DENABLE_UNIT_MAP_ACCESS=1
        )
        if(NOT UNITS_DISABLE_EXTRA_UNIT_STANDARDS)
            target_compile_definitions(test_r20 PUBLIC -DENABLE_UNIT_MAP_ACCESS=1)
        endif()

        add_unit_test(test_leadingNumbers.cpp)
        target_link_libraries(
            test_leadingNumbers ${UNITS_LC_PROJECT_NAME}::units compile_flags_target
        )
        if(UNITS_CLANG_TIDY)
            set_property(TARGET ${T} PROPERTY CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
        endif()
        target_compile_definitions(test_leadingNumbers PUBLIC -DENABLE_UNIT_TESTING=1)
        if(MSVC)
            target_compile_options(test_leadingNumbers PRIVATE /wd4459)
        endif()
    endif()

    target_compile_definitions(
        test_conversions2 PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}"
    )
    target_compile_definitions(
        fuzz_issue_tests PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}"
    )

    target_compile_definitions(
        test_siunits PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}"
    )

    target_sources(
        test_ucum PRIVATE ${PROJECT_SOURCE_DIR}/ThirdParty/xml/tinyxml2.cpp
                          ${PROJECT_SOURCE_DIR}/ThirdParty/xml/tinyxml2.h
    )
    target_include_directories(test_ucum PRIVATE ${PROJECT_SOURCE_DIR}/ThirdParty)
    target_compile_definitions(
        test_ucum PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}"
    )

    target_sources(
        test_udunits PRIVATE ${PROJECT_SOURCE_DIR}/ThirdParty/xml/tinyxml2.cpp
                             ${PROJECT_SOURCE_DIR}/ThirdParty/xml/tinyxml2.h
    )
    target_include_directories(test_udunits PRIVATE ${PROJECT_SOURCE_DIR}/ThirdParty)
    target_compile_definitions(
        test_udunits PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}"
    )

    target_compile_definitions(
        test_google_units PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}"
    )

    target_compile_definitions(
        test_complete_unit_list PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}"
    )
    if(NOT MSVC)
        target_compile_options(test_ucum PRIVATE -Wno-double-promotion)
        target_compile_options(test_udunits PRIVATE -Wno-double-promotion)
    else()
        target_compile_options(test_unit_ops PRIVATE /wd4127)
        target_compile_options(test_random_round_trip PRIVATE /wd4127)
    endif()

    if(UNITS_CLANG_TIDY)
        set_property(TARGET test_ucum PROPERTY CXX_CLANG_TIDY "")
        set_property(TARGET test_udunits PROPERTY CXX_CLANG_TIDY "")
    endif()

    add_executable(examples_test examples_test.cpp)
    target_link_libraries(
        examples_test PRIVATE ${UNITS_LC_PROJECT_NAME}::units compile_flags_target
    )
    target_compile_definitions(examples_test PRIVATE ${TARGET_SPECIAL_COMPILER_FLAGS})
    set_target_properties(examples_test PROPERTIES FOLDER "Tests")
    add_test(examples_test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/examples_test)

    if(WIN32)
        add_executable(examples_test_windows examples_test_windows.cpp)
        target_link_libraries(
            examples_test_windows PRIVATE ${UNITS_LC_PROJECT_NAME}::units
                                          compile_flags_target
        )
        target_compile_definitions(
            examples_test_windows PRIVATE ${TARGET_SPECIAL_COMPILER_FLAGS}
        )
        set_target_properties(examples_test_windows PROPERTIES FOLDER "Tests")
        add_test(examples_test_windows ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/examples_test)
    endif()

    add_executable(test_all_unit_base test_all_unit_base.cpp)
    if(MSVC)
        target_compile_options(test_all_unit_base PRIVATE /wd4459)
    endif()
    target_link_libraries(
        test_all_unit_base gtest gmock gtest_main ${UNITS_LC_PROJECT_NAME}::units
        compile_flags_target
    )
    set_target_properties(test_all_unit_base PROPERTIES FOLDER "Tests")
    # test_all_unit_base should not be added to the tests it will take a long time run

    if(CMAKE_BUILD_TYPE STREQUAL Coverage)
        include(CodeCoverage)
        setup_target_for_coverage_lcov(
            NAME
            UNITS_coverage
            EXECUTABLE
            ctest
            DEPENDENCIES
            ${UNITS_TESTS}
            examples_test
        )
    endif()

    if(UNITS_INSTALL_PACKAGE_TESTS)
        set(UNITS_PACKAGE_SEARCH_LOC)

        if(NOT MSVC)
            set(package_test_command --test-command "${CMAKE_CTEST_COMMAND}")
        else() # don't try to run the tests on MSVC since that would require copying the
               # dll's and doing some other setup that isn't that important to run on
               # all OS
            set(package_test_command)
        endif()

        if(CMAKE_BUILD_TYPE)
            set(UNITS_PACKAGE_TEST_BUILD_TYPE ${CMAKE_BUILD_TYPE})
        else()
            set(UNITS_PACKAGE_TEST_BUILD_TYPE Release)
        endif()

        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/find_package_tests")

        if(MSVC AND ${CMAKE_VERSION} VERSION_GREATER 3.12.9)
            # Tests for other CMake projects including and using UNITS using
            # find_package
            add_test(
                NAME find-package-testsA
                COMMAND
                    ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -A
                    "${CMAKE_GENERATOR_PLATFORM}" ${UNITS_PACKAGE_SEARCH_LOC}
                    "-DUNITS_BUILD_SHARED_LIBRARY=${UNITS_BUILD_SHARED_LIBRARY}"
                    "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
                    "-DUNITS_TARGET=${UNITS_LC_PROJECT_NAME}"
                    "-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}"
                    "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/find_package_tests"
                WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/find_package_tests"
            )
        else()
            add_test(
                NAME find-package-testsA
                COMMAND
                    ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${UNITS_PACKAGE_SEARCH_LOC}
                    "-DUNITS_BUILD_SHARED_LIBRARY=${UNITS_BUILD_SHARED_LIBRARY}"
                    "-DUNITS_TARGET=${UNITS_LC_PROJECT_NAME}"
                    "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
                    "-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}"
                    "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/find_package_tests"
                WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/find_package_tests"
            )
        endif()

        add_test(
            NAME find-package-testsB
            COMMAND
                ${CMAKE_COMMAND} --build
                "${CMAKE_CURRENT_BINARY_DIR}/find_package_tests" --config
                ${UNITS_PACKAGE_TEST_BUILD_TYPE}
        )

        add_test(
            NAME find-package-testsC
            COMMAND ${CMAKE_CTEST_COMMAND} -C ${UNITS_PACKAGE_TEST_BUILD_TYPE}
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/find_package_tests"
        )
    endif() # UNITS_INSTALL_PACKAGE_TESTS
endif() # UNITS_HEADER_ONLY
