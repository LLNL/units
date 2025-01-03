# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2019-2025,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

if(UNITS_DISABLE_EXTRA_UNIT_STANDARDS)
    set(units_source_files units.cpp commodities.cpp)
else()
    set(units_source_files units.cpp commodities.cpp x12_conv.cpp r20_conv.cpp)
endif()

set(units_header_files
    units.hpp
    units_decl.hpp
    unit_definitions.hpp
    units_util.hpp
    units_conversion_maps.hpp
    units_math.hpp
    commodity_definitions.hpp
    commodity_conversion_maps.hpp
)

include(GenerateExportHeader)

if(UNITS_DOMAIN)
    if(${UNITS_DOMAIN} MATCHES "domains::")
        set(UNITS_DEFAULT_DOMAIN ${UNITS_DOMAIN})
    elseif(${UNITS_DOMAIN} STREQUAL "UCUM")
        set(UNITS_DEFAULT_DOMAIN "domains::strict_ucum")
    elseif(${UNITS_DOMAIN} STREQUAL "COOKING")
        set(UNITS_DEFAULT_DOMAIN "domains::cooking")
    elseif(${UNITS_DOMAIN} STREQUAL "ASTRONOMY")
        set(UNITS_DEFAULT_DOMAIN "domains::astronomy")
    elseif(${UNITS_DOMAIN} STREQUAL "SURVEYING")
        set(UNITS_DEFAULT_DOMAIN "domains::surveying")
    elseif(${UNITS_DOMAIN} STREQUAL "NUCLEAR")
        set(UNITS_DEFAULT_DOMAIN "domains::nuclear")
    elseif(${UNITS_DOMAIN} STREQUAL "US_CUSTOMARY")
        set(UNITS_DEFAULT_DOMAIN "domains::us_customary")
    elseif(${UNITS_DOMAIN} STREQUAL "CLIMATE")
        set(UNITS_DEFAULT_DOMAIN "domains::climate")
    endif()
endif()

if(UNITS_BUILD_SHARED_LIBRARY)
    add_library(units SHARED ${units_source_files} ${units_header_files})
    generate_export_header(units BASE_NAME units)
    target_compile_definitions(units PUBLIC UNITS_EXPORT_HEADER)
    target_include_directories(
        units
        PUBLIC $<BUILD_INTERFACE:${${UNITS_CMAKE_PROJECT_NAME}_SOURCE_DIR}>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
               $<BUILD_INTERFACE:${${UNITS_CMAKE_PROJECT_NAME}_BINARY_DIR}>
               $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_link_libraries(units PRIVATE compile_flags_target)

    if(UNITS_NAMESPACE)
        target_compile_definitions(units PUBLIC -DUNITS_NAMESPACE=${UNITS_NAMESPACE})
    endif()
    if(UNITS_BASE_TYPE)
        target_compile_definitions(units PUBLIC -DUNITS_BASE_TYPE=${UNITS_BASE_TYPE})
    endif()
    if(UNITS_DISABLE_NON_ENGLISH_UNITS)
        target_compile_definitions(units PUBLIC -DUNITS_DISABLE_NON_ENGLISH_UNITS=1)
    endif()
    if(UNITS_DISABLE_EXTRA_UNIT_STANDARDS)
        target_compile_definitions(units PUBLIC -DUNITS_DISABLE_EXTRA_UNIT_STANDARDS=1)
    endif()
    if(UNITS_DEFAULT_DOMAIN)
        target_compile_definitions(
            units PRIVATE -DUNITS_DEFAULT_DOMAIN=${UNITS_DEFAULT_DOMAIN}
        )
    endif()

    if(UNITS_DEFAULT_MATCH_FLAGS)
        target_compile_definitions(
            units PRIVATE -DUNITS_DEFAULT_MATCH_FLAGS=${UNITS_DEFAULT_MATCH_FLAGS}
        )
    endif()

    if(CMAKE_CXX_STANDARD GREATER 16)
        target_compile_definitions(units PRIVATE -DUNITS_CONSTEXPR_IF_SUPPORTED=1)
    endif()
    if(UNITS_INSTALL AND NOT SKBUILD)
        install(
            TARGETS units ${UNITS_LIBRARY_EXPORT_COMMAND}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # INCLUDES DESTINATION
                                                        # include/units
        )
    endif()
    add_library(${UNITS_LC_PROJECT_NAME}::units ALIAS units)

elseif(UNITS_BUILD_OBJECT_LIBRARY)
    add_library(units OBJECT ${units_source_files} ${units_header_files})
    target_include_directories(
        units PRIVATE $<BUILD_INTERFACE:${${UNITS_CMAKE_PROJECT_NAME}_SOURCE_DIR}>
    )

    if(UNITS_NAMESPACE)
        target_compile_definitions(units PUBLIC -DUNITS_NAMESPACE=${UNITS_NAMESPACE})
    endif()
    if(UNITS_BASE_TYPE)
        target_compile_definitions(units PUBLIC -DUNITS_BASE_TYPE=${UNITS_BASE_TYPE})
    endif()
    if(UNITS_DEFAULT_DOMAIN)
        target_compile_definitions(
            units PRIVATE -DUNITS_DEFAULT_DOMAIN=${UNITS_DEFAULT_DOMAIN}
        )
    endif()
    if(UNITS_DISABLE_NON_ENGLISH_UNITS)
        target_compile_definitions(units PUBLIC -DUNITS_DISABLE_NON_ENGLISH_UNITS=1)
    endif()
    if(UNITS_DISABLE_EXTRA_UNIT_STANDARDS)
        target_compile_definitions(units PUBLIC -DUNITS_DISABLE_EXTRA_UNIT_STANDARDS=1)
    endif()
    if(CMAKE_CXX_STANDARD GREATER 16)
        target_compile_definitions(units PRIVATE -DUNITS_CONSTEXPR_IF_SUPPORTED=1)
    endif()
elseif(UNITS_BUILD_STATIC_LIBRARY)
    add_library(units STATIC ${units_source_files} ${units_header_files})
    target_include_directories(
        units
        PUBLIC $<BUILD_INTERFACE:${${UNITS_CMAKE_PROJECT_NAME}_SOURCE_DIR}>
               $<BUILD_INTERFACE:${${UNITS_CMAKE_PROJECT_NAME}_BINARY_DIR}>
               $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_link_libraries(units PRIVATE compile_flags_target)

    set_target_properties(units PROPERTIES POSITION_INDEPENDENT_CODE ON)
    if(UNITS_ENABLE_TESTS)
        target_compile_definitions(
            units PUBLIC -DENABLE_UNIT_TESTING=1 -DENABLE_UNIT_MAP_ACCESS=1
        )
    endif()
    if(UNITS_NAMESPACE)
        target_compile_definitions(units PUBLIC -DUNITS_NAMESPACE=${UNITS_NAMESPACE})
    endif()
    if(UNITS_BASE_TYPE)
        target_compile_definitions(units PUBLIC -DUNITS_BASE_TYPE=${UNITS_BASE_TYPE})
    endif()
    if(UNITS_DISABLE_EXTRA_UNIT_STANDARDS)
        target_compile_definitions(units PUBLIC -DUNITS_DISABLE_EXTRA_UNIT_STANDARDS=1)
    endif()
    if(UNITS_DEFAULT_DOMAIN)
        target_compile_definitions(
            units PRIVATE -DUNITS_DEFAULT_DOMAIN=${UNITS_DEFAULT_DOMAIN}
        )
    endif()
    if(UNITS_DISABLE_NON_ENGLISH_UNITS)
        target_compile_definitions(units PUBLIC -DUNITS_DISABLE_NON_ENGLISH_UNITS=1)
    endif()
    if(CMAKE_CXX_STANDARD GREATER 16)
        target_compile_definitions(units PRIVATE -DUNITS_CONSTEXPR_IF_SUPPORTED=1)
    endif()
    if(UNITS_INSTALL
       AND NOT UNITS_BINARY_ONLY_INSTALL
       AND NOT SKBUILD
    )
        install(TARGETS units ${UNITS_LIBRARY_EXPORT_COMMAND}
                DESTINATION ${CMAKE_INSTALL_LIBDIR} # INCLUDES DESTINATION
                                                    # include/${PROJECT_NAME}
        )
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL Coverage)
        set_source_files_properties(foo.cpp PROPERTIES COMPILE_FLAGS -Wno-effc++)
    endif()
    if(UNITS_CLANG_TIDY)
        set_property(TARGET units PROPERTY CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
    endif()
    add_library(${UNITS_LC_PROJECT_NAME}::units ALIAS units)
    message(STATUS "adding library ${UNITS_LC_PROJECT_NAME}::units")
endif()

# add the header only interface library
add_library(header_only INTERFACE)
target_include_directories(
    header_only INTERFACE $<BUILD_INTERFACE:${${UNITS_CMAKE_PROJECT_NAME}_SOURCE_DIR}>
                          $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_definitions(header_only INTERFACE UNITS_HEADER_ONLY)
if(UNITS_INSTALL
   AND NOT UNITS_BINARY_ONLY_
   AND NOT SKBUILD
)
    install(TARGETS header_only ${UNITS_LIBRARY_EXPORT_COMMAND})
endif()
if(UNITS_NAMESPACE)
    target_compile_definitions(
        header_only INTERFACE -DUNITS_NAMESPACE=${UNITS_NAMESPACE}
    )
endif()
if(UNITS_BASE_TYPE)
    target_compile_definitions(
        header_only INTERFACE -DUNITS_BASE_TYPE=${UNITS_BASE_TYPE}
    )
endif()
add_library(${UNITS_LC_PROJECT_NAME}::header_only ALIAS header_only)

if(UNITS_INSTALL
   AND NOT UNITS_BINARY_ONLY_INSTALL
   AND NOT SKBUILD
)
    install(FILES ${units_header_files}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${UNITS_LC_PROJECT_NAME}
    )
    if(UNITS_BUILD_SHARED_LIBRARY OR BUILD_SHARED_LIBS)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/units_export.h
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${UNITS_LC_PROJECT_NAME}
        )
    endif()
endif()
