if (NOT TARGET GTest::Main)

find_package(GTest)

if (NOT GTest)
	file(MAKE_DIRECTORY ${directory} ${CMAKE_BINARY_DIR}/googletest-download)
	set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE BOOL "")
	configure_file(download_gtest.in ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt)
	execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -A "${CMAKE_GENERATOR_PLATFORM}" ${CMAKE_BINARY_DIR}/googletest-download
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
	execute_process(COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/googletest-download
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
	
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(GTEST_FOUND ON CACHE BOOL "" FORCE)
    if (MSYS)
	   set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)
    endif()

    if (MSVC)
       add_compile_options( /wd4459)
    endif()
	add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src ${CMAKE_BINARY_DIR}/googletest-src-build)
	add_library(GTest::Main ALIAS gtest_main)
	
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
	
	set(GTEST_FOUND ON CACHE BOOL "" FORCE)
endif()
endif()

function(add_unit_test test_source_file)
  get_filename_component(test_name "${test_source_file}" NAME_WE)
  add_executable("${test_name}" "${test_source_file}")
  target_link_libraries("${test_name}" GTest::Main units)
  add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${test_name}>)
   set_target_properties(${test_name} PROPERTIES FOLDER "Tests")
endfunction()
