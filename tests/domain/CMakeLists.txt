cmake_minimum_required(VERSION 3.20)
project(domaintests)

add_library(domaintests_lib OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/CircuitTest.cpp)

get_target_property(DOCTEST_INCLUDE_DIRS doctest::doctest
                    INTERFACE_INCLUDE_DIRECTORIES)

target_include_directories(domaintests_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                                  ${DOCTEST_INCLUDE_DIRS})

target_link_libraries(domaintests_lib domain_lib viewmodel_lib
                      component_library test_helpers)
