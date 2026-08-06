cmake_minimum_required(VERSION 3.20)
project(circuitVMtests)

set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(circuitVMtests SHARED ${SOURCES_DIR}/CircuitViewModelTest.cpp)

target_include_directories(circuitVMtests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(circuitVMtests PRIVATE viewmodel_lib doctest_with_main)
