include(UnitTestingHelpers)

set(TEST_FILE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/files)
add_unit_test(test_measurement.cpp)
add_unit_test(test_unit_strings.cpp)
add_unit_test(test_unit_ops.cpp)
add_unit_test(test_pu.cpp)
add_unit_test(test_equation_units.cpp)
add_unit_test(test_conversions1.cpp)
add_unit_test(test_conversions2.cpp)
add_unit_test(fuzz_issue_tests.cpp)
add_unit_test(test_random_round_trip.cpp)

target_compile_definitions(test_unit_strings PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}")
target_compile_definitions(test_conversions2 PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}")
target_compile_definitions(fuzz_issue_tests PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}")

add_unit_test(test_ucum.cpp)

target_sources(test_ucum PRIVATE ${CMAKE_SOURCE_DIR}/ThirdParty/xml/tinyxml2.cpp ${CMAKE_SOURCE_DIR}/ThirdParty/xml/tinyxml2.h)
target_include_directories(test_ucum PRIVATE ${CMAKE_SOURCE_DIR}/ThirdParty)
target_compile_definitions(test_ucum PUBLIC -DTEST_FILE_FOLDER="${TEST_FILE_FOLDER}")
