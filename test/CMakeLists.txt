# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2019,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

include(AddGoogletest)

set(UNITS_TESTS
    fuzz_issue_tests
    test_conversions1
    test_conversions2
    test_equation_units
    test_measurement
	test_pu
    test_random_round_trip
    test_ucum
    test_udunits
    test_unit_ops
    test_unit_strings
	test_measurement_strings
	test_commodities
	test_leadingNumbers
    )
	
set(TEST_FILE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/files)

foreach(T ${UNITS_TESTS})

    add_unit_test(${T}.cpp)
    target_link_libraries(${T} units::units compile_flags_target)

endforeach()

target_compile_definitions(test_unit_strings PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}")
target_compile_definitions(test_conversions2 PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}")
target_compile_definitions(fuzz_issue_tests PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}")

target_sources(test_ucum PRIVATE ${CMAKE_SOURCE_DIR}/ThirdParty/xml/tinyxml2.cpp ${CMAKE_SOURCE_DIR}/ThirdParty/xml/tinyxml2.h)
target_include_directories(test_ucum PRIVATE ${CMAKE_SOURCE_DIR}/ThirdParty)
target_compile_definitions(test_ucum PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}")


target_sources(test_udunits PRIVATE ${CMAKE_SOURCE_DIR}/ThirdParty/xml/tinyxml2.cpp ${CMAKE_SOURCE_DIR}/ThirdParty/xml/tinyxml2.h)
target_include_directories(test_udunits PRIVATE ${CMAKE_SOURCE_DIR}/ThirdParty)
target_compile_definitions(test_udunits PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}")
if (NOT MSVC)
    target_compile_options(test_ucum PRIVATE -Wno-double-promotion)
	target_compile_options(test_udunits PRIVATE -Wno-double-promotion)
endif()
add_executable(examples_test examples_test.cpp)
target_link_libraries(examples_test PRIVATE units::units)
target_compile_definitions(examples_test PRIVATE ${TARGET_SPECIAL_COMPILER_FLAGS})
set_target_properties(examples_test PROPERTIES FOLDER "Tests")
add_test(examples_test examples_test)

if(CMAKE_BUILD_TYPE STREQUAL Coverage)
    include(CodeCoverage)
    setup_target_for_coverage(
        NAME UNITS_coverage
        EXECUTABLE ctest
        DEPENDENCIES
          ${UNITS_TESTS}
		  examples_test
          )
endif()