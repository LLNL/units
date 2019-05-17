
add_executable(fuzz_from_string fuzz_target_from_string.cpp)
  target_link_libraries(fuzz_from_string unitslib)
  target_include_directories(fuzz_from_string PRIVATE ${CMAKE_SOURCE_DIR}/ThirdParty)
  
