cmake_minimum_required(VERSION 3.20)
project(tests)

add_subdirectory(doctest)
add_subdirectory(viewmodel)
add_subdirectory(domain)
add_subdirectory(integration)
add_subdirectory(helpers)

add_executable(
  photonics_tests main_test.cpp $<TARGET_OBJECTS:viewmodeltests_lib>
                  $<TARGET_OBJECTS:domaintests_lib>)

target_include_directories(photonics_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
  photonics_tests PRIVATE doctest::doctest domain_lib viewmodel_lib
                          component_library test_helpers)

add_test(NAME photonics_tests COMMAND photonics_tests)
