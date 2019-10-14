# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2019,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set(units_source_files
  units.cpp
  x12_conv.cpp
  r20_conv.cpp
  commodities.cpp
)

set(units_header_files
  units.hpp
  units_decl.hpp
  unit_definitions.hpp
)

option(UNITS_BINARY_ONLY_INSTALL OFF "only install the binary files")
mark_as_advanced(UNITS_BINARY_ONLY_INSTALL)

if (UNITS_HEADER_ONLY)
  # TODO:
  # install units_Sources
  # add this directory to the include path
else (UNITS_HEADER_ONLY)
if (UNITS_BUILD_STATIC_LIBRARY)
	add_library(units-static STATIC ${units_source_files} ${units_header_files})
	target_include_directories(units-static PUBLIC  
     $<BUILD_INTERFACE:${UNITS_SOURCE_DIR}>
     $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

	add_library(units::units ALIAS units-static)
	add_library(units::static ALIAS units-static)
	if (NOT UNITS_BINARY_ONLY_INSTALL)
	install(TARGETS units-static
		${UNITS_LIBRARY_EXPORT_COMMAND}
		DESTINATION ${CMAKE_INSTALL_LIBDIR}
		# INCLUDES DESTINATION include/units
		)
	endif(NOT UNITS_BINARY_ONLY_INSTALL)
endif (UNITS_BUILD_STATIC_LIBRARY)

if (UNITS_BUILD_SHARED_LIBRARY)
	add_library(units-shared SHARED ${units_source_files} ${units_header_files})
	target_include_directories(units-shared PUBLIC  
     $<BUILD_INTERFACE:${UNITS_SOURCE_DIR}>
     $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

	 if (NOT UNITS_BUILD_STATIC_LIBRARY)
		add_library(units::units ALIAS units-shared)
	endif()
	add_library(units::shared ALIAS units-shared)
	install(TARGETS units-shared
		${UNITS_LIBRARY_EXPORT_COMMAND}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		# INCLUDES DESTINATION include/units
		)
endif (UNITS_BUILD_SHARED_LIBRARY)

if (UNITS_BUILD_OBJECT_LIBRARY)
	add_library(units-object OBJECT ${units_source_files} ${units_header_files})
	target_include_directories(units-object PRIVATE  
     $<BUILD_INTERFACE:${UNITS_SOURCE_DIR}>
     )

	add_library(units::object ALIAS units-object)

	
endif (UNITS_BUILD_OBJECT_LIBRARY)

endif(UNITS_HEADER_ONLY)

if (NOT UNITS_BINARY_ONLY_INSTALL)
install (FILES ${units_header_files} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()