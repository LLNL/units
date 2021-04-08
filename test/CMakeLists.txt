# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2019-2021,
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
    test_leadingNumbers
    test_siunits
)

set(TEST_FILE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/files)

if(UNITS_HEADER_ONLY)

    foreach(T ${UNIT_TEST_HEADER_ONLY})

        add_unit_test(${T}.cpp)
        target_link_libraries(${T} units::units-header-only compile_flags_target)
    endforeach()

    if(MSVC)
        target_compile_options(test_unit_ops PRIVATE /wd4127)
    endif()
else()
    foreach(T ${UNITS_TESTS})

        add_unit_test(${T}.cpp)
        target_link_libraries(${T} units::units compile_flags_target)
        if(UNITS_CLANG_TIDY)
            set_property(TARGET ${T} PROPERTY CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
        endif()
    endforeach()

    target_compile_definitions(
        test_unit_strings PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}"
    )

    target_compile_definitions(
        test_unit_strings PUBLIC -DENABLE_UNIT_TESTING=1 -DENABLE_UNIT_MAP_ACCESS=1
    )
    target_compile_definitions(test_leadingNumbers PUBLIC -DENABLE_UNIT_TESTING=1)

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
    target_link_libraries(examples_test PRIVATE units::units)
    target_compile_definitions(examples_test PRIVATE ${TARGET_SPECIAL_COMPILER_FLAGS})
    set_target_properties(examples_test PROPERTIES FOLDER "Tests")
    add_test(examples_test examples_test)

    add_executable(test_all_unit_base test_all_unit_base.cpp)

    target_link_libraries(
        test_all_unit_base gtest gmock gtest_main units::units compile_flags_target
    )
    set_target_properties(test_all_unit_base PROPERTIES FOLDER "Tests")
    # test_all_unit_base should not be added to the tests it will take a long time run

    if(CMAKE_BUILD_TYPE STREQUAL Coverage)
        include(CodeCoverage)
        setup_target_for_coverage(
            NAME
            UNITS_coverage
            EXECUTABLE
            ctest
            DEPENDENCIES
            ${UNITS_TESTS}
            examples_test
        )
    endif()

endif() # UNITS_HEADER_ONLY
