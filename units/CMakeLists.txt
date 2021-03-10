# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2019-2021,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set(units_source_files units.cpp x12_conv.cpp r20_conv.cpp commodities.cpp)

set(units_header_files units.hpp units_decl.hpp unit_definitions.hpp units_util.hpp)

include(GenerateExportHeader)

if(UNITS_HEADER_ONLY)
    add_library(units-header-only INTERFACE)
    target_include_directories(
        units-header-only INTERFACE $<BUILD_INTERFACE:${UNITS_SOURCE_DIR}>
                                    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_compile_definitions(units-header-only INTERFACE UNITS_HEADER_ONLY)
    if(UNITS_INSTALL)
        install(TARGETS units-header-only ${UNITS_LIBRARY_EXPORT_COMMAND})
    endif()
    if(UNITS_NAMESPACE)
        target_compile_definitions(
            units-header-only INTERFACE -DUNITS_NAMESPACE=${UNITS_NAMESPACE}
        )
    endif()
    add_library(units::units-header-only ALIAS units-header-only)
else(UNITS_HEADER_ONLY)
    if(UNITS_BUILD_STATIC_LIBRARY)
        add_library(units-static STATIC ${units_source_files} ${units_header_files})
        target_include_directories(
            units-static PUBLIC $<BUILD_INTERFACE:${UNITS_SOURCE_DIR}>
                                $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
                                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )
        target_link_libraries(units-static PRIVATE compile_flags_target)

        if(UNITS_ENABLE_TESTS)
            target_compile_definitions(
                units-static PUBLIC -DENABLE_UNIT_TESTING=1 -DENABLE_UNIT_MAP_ACCESS=1
            )
        endif()
        if(UNITS_NAMESPACE)
            target_compile_definitions(
                units-static PUBLIC -DUNITS_NAMESPACE=${UNITS_NAMESPACE}
            )
        endif()
        add_library(units::units ALIAS units-static)
        add_library(units::static ALIAS units-static)
        if(UNITS_INSTALL)
            install(TARGETS units-static ${UNITS_LIBRARY_EXPORT_COMMAND}
                    DESTINATION ${CMAKE_INSTALL_LIBDIR} # INCLUDES DESTINATION
                                                        # include/units
            )
        endif(UNITS_INSTALL)
        if(CMAKE_BUILD_TYPE STREQUAL Coverage)
            set_source_files_properties(foo.cpp PROPERTIES COMPILE_FLAGS -Wno-effc++)
        endif()
        if(UNITS_CLANG_TIDY)
            set_property(TARGET units-static PROPERTY CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
        endif()
    endif(UNITS_BUILD_STATIC_LIBRARY)

    if(UNITS_BUILD_SHARED_LIBRARY)
        add_library(units-shared SHARED ${units_source_files} ${units_header_files})
        generate_export_header(units-shared BASE_NAME units)
        target_compile_definitions(units-shared PUBLIC UNITS_EXPORT_HEADER)
        target_include_directories(
            units-shared PUBLIC $<BUILD_INTERFACE:${UNITS_SOURCE_DIR}>
                                $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
                                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )
        target_link_libraries(units-shared PRIVATE compile_flags_target)
        if(NOT UNITS_BUILD_STATIC_LIBRARY)
            add_library(units::units ALIAS units-shared)
        endif()
        add_library(units::shared ALIAS units-shared)
        if(UNITS_NAMESPACE)
            target_compile_definitions(
                units-shared PUBLIC -DUNITS_NAMESPACE=${UNITS_NAMESPACE}
            )
        endif()
        if(UNITS_INSTALL)
            install(
                TARGETS units-shared ${UNITS_LIBRARY_EXPORT_COMMAND}
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # INCLUDES DESTINATION
                                                            # include/units
            )
        endif()
    endif(UNITS_BUILD_SHARED_LIBRARY)

    if(UNITS_BUILD_OBJECT_LIBRARY)
        add_library(units-object OBJECT ${units_source_files} ${units_header_files})
        target_include_directories(
            units-object PRIVATE $<BUILD_INTERFACE:${UNITS_SOURCE_DIR}>
        )

        add_library(units::object ALIAS units-object)
        if(UNITS_NAMESPACE)
            target_compile_definitions(
                units-object PUBLIC -DUNITS_NAMESPACE=${UNITS_NAMESPACE}
            )
        endif()
    endif(UNITS_BUILD_OBJECT_LIBRARY)

endif(UNITS_HEADER_ONLY)

if(UNITS_INSTALL AND NOT UNITS_BINARY_ONLY_INSTALL)
    install(FILES ${units_header_files} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()
