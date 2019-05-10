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
    test_unit_ops
    test_unit_strings
    )
	
set(TEST_FILE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/files)

foreach(T ${UNITS_TESTS})

    add_unit_test(${T}.cpp)
    target_link_libraries(${T} unitslib)

endforeach()

target_compile_definitions(test_unit_strings PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}")
target_compile_definitions(test_conversions2 PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}")
target_compile_definitions(fuzz_issue_tests PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}")

target_sources(test_ucum PRIVATE ${CMAKE_SOURCE_DIR}/ThirdParty/xml/tinyxml2.cpp ${CMAKE_SOURCE_DIR}/ThirdParty/xml/tinyxml2.h)
target_include_directories(test_ucum PRIVATE ${CMAKE_SOURCE_DIR}/ThirdParty)
target_compile_definitions(test_ucum PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}")

if(CMAKE_BUILD_TYPE STREQUAL Coverage)
    include(CodeCoverage)
    setup_target_for_coverage(
        NAME UNITS_coverage
        EXECUTABLE ctest
        DEPENDENCIES
          ${UNITS_TESTS}
          )
endif()