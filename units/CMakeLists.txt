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

if (UNITS_HEADER_ONLY)
  # TODO:
  # install units_Sources
  # add this directory to the include path
else (UNITS_HEADER_ONLY)
if (BUILD_UNITS_STATIC_LIBRARY)
	add_library(units-static STATIC ${units_source_files} ${units_header_files})
	target_include_directories(units-static PUBLIC  
     $<BUILD_INTERFACE:${UNITS_SOURCE_DIR}>
     $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

	add_library(units::units ALIAS units-static)
	add_library(units::static ALIAS units-static)
	install(TARGETS units-static
		${UNITS_LIBRARY_EXPORT_COMMAND}
		DESTINATION ${CMAKE_INSTALL_LIBDIR}
		# INCLUDES DESTINATION include/units
		)
endif (BUILD_UNITS_STATIC_LIBRARY)

if (BUILD_UNITS_SHARED_LIBRARY)
	add_library(units-shared SHARED ${units_source_files} ${units_header_files})
	target_include_directories(units-shared PUBLIC  
     $<BUILD_INTERFACE:${UNITS_SOURCE_DIR}>
     $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

	 if (NOT BUILD_UNITS_STATIC_LIBRARY)
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
endif (BUILD_UNITS_SHARED_LIBRARY)

if (BUILD_UNITS_OBJECT_LIBRARY)
	add_library(units-object OBJECT ${units_source_files} ${units_header_files})
	target_include_directories(units-object PRIVATE  
     $<BUILD_INTERFACE:${UNITS_SOURCE_DIR}>
     )

	add_library(units::object ALIAS units-object)

	
endif (BUILD_UNITS_OBJECT_LIBRARY)

endif(UNITS_HEADER_ONLY)


install (FILES ${units_header_files} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})