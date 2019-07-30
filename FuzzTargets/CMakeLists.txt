
add_executable(fuzz_from_string fuzz_target_from_string.cpp)
  target_link_libraries(fuzz_from_string units::units)
  target_include_directories(fuzz_from_string PRIVATE ${CMAKE_SOURCE_DIR}/ThirdParty)
  
add_executable(fuzz_with_flags fuzz_target_from_string_flags.cpp)
  target_link_libraries(fuzz_with_flags units::units)
  target_include_directories(fuzz_with_flags PRIVATE ${CMAKE_SOURCE_DIR}/ThirdParty)