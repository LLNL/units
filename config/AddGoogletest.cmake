# 
#
# Downloads GTest and provides a helper macro to add tests. Add make check, as well, which
# gives output on failed tests without having to set an environment variable.
#
#
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF)
# older version of google tests doesn't support MSYS so needs this flag to compile
if (MSYS)
	set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)
endif()
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE BOOL "")
add_subdirectory("${UNITS_SOURCE_DIR}/ThirdParty/googletest" "${UNITS_BINARY_DIR}/ThirdParty/googletest" EXCLUDE_FROM_ALL)


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


mark_as_advanced(
gmock_build_tests
gtest_build_samples
gtest_build_tests
gtest_disable_pthreads
gtest_force_shared_crt
gtest_hide_internal_symbols
BUILD_GMOCK
BUILD_GTEST
)

set_target_properties(gtest gtest_main gmock gmock_main
    PROPERTIES FOLDER "Extern")

if(MSVC)
     #  add_compile_options( /wd4459)
    if (MSVC_VERSION GREATER_EQUAL 1900)
        target_compile_definitions(gtest PUBLIC _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
        target_compile_definitions(gtest_main PUBLIC _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
        target_compile_definitions(gmock PUBLIC _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
        target_compile_definitions(gmock_main PUBLIC _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
    endif()
endif()
