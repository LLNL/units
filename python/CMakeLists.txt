cmake_minimum_required(VERSION 3.22...3.31)
project(llnl-units) # Replace 'my_project' with the name of your project

find_package(
    Python 3.11
    COMPONENTS Interpreter Development.Module
    REQUIRED
)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE
        Release
        CACHE STRING "Choose the type of build." FORCE
    )
    set_property(
        CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel"
                                        "RelWithDebInfo"
    )
endif()

# Detect the installed nanobind package and import it into CMake
execute_process(
    COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE nanobind_ROOT
)
find_package(nanobind CONFIG REQUIRED)

nanobind_add_module(units_llnl units_python.cpp)
target_link_libraries(units_llnl PUBLIC units::units)
