# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2019-2025,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

add_executable(fuzz_from_string fuzz_target_from_string.cpp)
target_link_libraries(
    fuzz_from_string ${UNITS_LC_PROJECT_NAME}::units compile_flags_target
)
target_include_directories(fuzz_from_string PRIVATE ${CMAKE_SOURCE_DIR}/ThirdParty)

add_executable(fuzz_measurement fuzz_target_measurement_from_string.cpp)
target_link_libraries(
    fuzz_measurement ${UNITS_LC_PROJECT_NAME}::units compile_flags_target
)
target_include_directories(fuzz_measurement PRIVATE ${CMAKE_SOURCE_DIR}/ThirdParty)

add_custom_target(
    QUICK_RAW_FUZZ
    COMMAND ${CMAKE_COMMAND} -E make_directory corp2
    COMMAND
        fuzz_measurement corp2 -max_total_time=300 -max_len=512
        -dict=${CMAKE_CURRENT_SOURCE_DIR}/fuzz_dictionary.txt
        -exact_artifact_path=units_fail_measurement_artifact.txt
    COMMAND ${CMAKE_COMMAND} -E make_directory corp1
    COMMAND
        fuzz_from_string corp1 -max_total_time=300 -max_len=512
        -dict=${CMAKE_CURRENT_SOURCE_DIR}/fuzz_dictionary.txt
        -exact_artifact_path=units_fail_string_artifact.txt
)
