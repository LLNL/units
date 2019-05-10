#
# Copyright (c) 2017-2019, Battelle Memorial Institute; Lawrence Livermore
# National Security, LLC; Alliance for Sustainable Energy, LLC.
# See the top-level NOTICE for additional details. 
#All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

option(
    ENABLE_EXTRA_COMPILER_WARNINGS
    "disable compiler warning for ${CMAKE_PROJECT_NAME} build" ON
)
option(
    ENABLE_ERROR_ON_WARNINGS
    "generate a compiler error for any warning encountered" OFF
)

mark_as_advanced(ENABLE_EXTRA_COMPILER_WARNINGS)
mark_as_advanced(ENABLE_ERROR_ON_WARNINGS)

# -------------------------------------------------------------
# Setup compiler options and configurations
# -------------------------------------------------------------
message(STATUS "setting up for ${CMAKE_CXX_COMPILER_ID}")
if(UNIX)
    # Since default builds of boost library under Unix don't use CMake, turn off
    # using CMake build and find include/libs the regular way.
    if(ENABLE_ERROR_ON_WARNINGS)
        add_compile_options(-Werror)
    endif(ENABLE_ERROR_ON_WARNINGS)

    if(ENABLE_EXTRA_COMPILER_WARNINGS)
        add_compile_options(-Wall -pedantic)
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wextra>)
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wshadow>)
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wstrict-aliasing=1>)
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wunreachable-code>)
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>)
        # add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wredundant-decls>)
       # add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wundef>)
        if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
            # this produces a lot of noise on newer compilers
            # add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wstrict-
            # overflow=5>)
            add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wcast-align>)
        endif()
        # this options produces lots of warning but is useful for checking every
        # once in a while with Clang, GCC warning notices with this aren't as
        # useful add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wpadded>) add
        # some gnu specific options if the compiler is newer
        if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
            # this option produces a number of warnings in third party libraries
            # but useful for checking for any internal usages
            # add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>)
            add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wlogical-op>)
            if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6.0)
                add_compile_options(
                    $<$<COMPILE_LANGUAGE:CXX>:-Wduplicated-cond>
                )
                add_compile_options(
                    $<$<COMPILE_LANGUAGE:CXX>:-Wnull-dereference>
                )
            endif()
            if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0)
                add_compile_options(
                    $<$<COMPILE_LANGUAGE:CXX>:-Wimplicit-fallthrough=2>
                )
            endif()
            if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.9)
                add_compile_options(
                    $<$<COMPILE_LANGUAGE:CXX>:-Wclass-memaccess>
                )
            endif()
        endif()
		if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6.0)
                add_compile_options(-Wdocumentation -Wdocumentation-pedantic -Wno-documentation-deprecated-sync)
		    endif()
		endif()
    endif(ENABLE_EXTRA_COMPILER_WARNINGS)
else(UNIX)
    if(MINGW)
        if(ENABLE_ERROR_ON_WARNINGS)
            add_compile_options(-Werror)
        endif(ENABLE_ERROR_ON_WARNINGS)

        if(ENABLE_EXTRA_COMPILER_WARNINGS)
            add_compile_options(-Wall -pedantic)
            add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wextra>)
            add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wshadow>)
            add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wstrict-aliasing=1>)
            add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wunreachable-code>)
            # add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wstrict-
            # overflow=5>)
            add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>)
            # add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wredundant-decls>)
            add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wcast-align>)
            #add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wundef>)

            # this options produces lots of warning but is useful for checking
            # every once in a while with Clang, GCC warning notices with this
            # aren't as useful
            # add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wpadded>)
            if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
                # this option produces a number of warnings in third party
                # libraries but useful for checking for any internal usages
                # add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wold-style-
                # cast>)
                add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wlogical-op>)
                if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6.0)
                    add_compile_options(
                        $<$<COMPILE_LANGUAGE:CXX>:-Wduplicated-cond>
                    )
                    add_compile_options(
                        $<$<COMPILE_LANGUAGE:CXX>:-Wnull-dereference>
                    )
                endif()
                if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0)
                    add_compile_options(
                        $<$<COMPILE_LANGUAGE:CXX>:-Wimplicit-fallthrough=2>
                    )
                endif()
            endif()
			if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6.0)
                add_compile_options(-Wdocumentation -Wdocumentation-pedantic -Wno-documentation-deprecated-sync)
		    endif()
		endif()
        endif(ENABLE_EXTRA_COMPILER_WARNINGS)
    else(MINGW)
        # -------------------------------------------------------------
        # Extra definitions for visual studio
        # -------------------------------------------------------------
        if(MSVC)
            if(ENABLE_ERROR_ON_WARNINGS)
                add_compile_options(/WX)
            endif(ENABLE_ERROR_ON_WARNINGS)

            add_definitions(-D_CRT_SECURE_NO_WARNINGS)
            add_definitions(-D_SCL_SECURE_NO_WARNINGS)
            add_compile_options(/MP /EHsc)
            add_compile_options(/sdl)
            if(ENABLE_EXTRA_COMPILER_WARNINGS)
                add_compile_options(
                    -W4
                    /wd4065
                    /wd4101
                    /wd4102
                    /wd4244
                    /wd4297
                    /wd4355
                    /wd4800
                    /wd4484
                    /wd4702
                    /wd4996
                )
            endif(ENABLE_EXTRA_COMPILER_WARNINGS)
            add_definitions(-D_WIN32_WINNT=0x0601)
        endif(MSVC)
    endif(MINGW)
endif(UNIX)

include(CheckCXXCompilerFlag)

if (CMAKE_CXX_STANDARD EQUAL 20)
  check_cxx_compiler_flag(-std=c++20 has_std_20_flag)
  if (has_std_20_flag)
    set(CXX_STANDARD_FLAG -std=c++20)
  else()
	check_cxx_compiler_flag(-std=c++2a has_std_2a_flag)
	if (has_std_2a_flag)
		set(CXX_STANDARD_FLAG -std=c++2a)
	endif()
  endif ()
elseif (CMAKE_CXX_STANDARD EQUAL 17)
  check_cxx_compiler_flag(-std=c++17 has_std_17_flag)
  if (has_std_17_flag)
    set(CXX_STANDARD_FLAG -std=c++17)
  else()
	check_cxx_compiler_flag(-std=c++1z has_std_1z_flag)
	if (has_std_1z_flag)
		set(CXX_STANDARD_FLAG -std=c++1z)
	endif ()
	endif()
elseif (CMAKE_CXX_STANDARD EQUAL 14)
  check_cxx_compiler_flag(-std=c++14 has_std_14_flag)

  if (has_std_14_flag)
    set(CXX_STANDARD_FLAG -std=c++14)
  else()
	check_cxx_compiler_flag(-std=c++1y has_std_1y_flag)
  if (has_std_1y_flag)
    set(CXX_STANDARD_FLAG -std=c++1y)
  endif ()
  endif()
#for C++11 cmake pretty much knows how to deal with that one
endif ()

set(CMAKE_REQUIRED_FLAGS ${CXX_STANDARD_FLAG})