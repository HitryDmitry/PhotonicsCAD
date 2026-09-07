cmake_minimum_required(VERSION 3.20)
project(test_helpers)

add_library(
  test_helpers OBJECT
  ${CMAKE_CURRENT_SOURCE_DIR}/TestHelpers.h
  ${CMAKE_CURRENT_SOURCE_DIR}/CircuitBuilder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/CircuitBuilder.cpp)

target_include_directories(test_helpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(test_helpers domain_lib component_library simulation_lib)
