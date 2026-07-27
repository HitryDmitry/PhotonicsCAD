cmake_minimum_required(VERSION 3.20)
project(viewmodel)

set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(
  viewmodel_lib SHARED
  ${SOURCES_DIR}/ComponentViewModel.cpp
  ${SOURCES_DIR}/ComponentViewModel.h
  ${SOURCES_DIR}/PinViewModel.cpp
  ${SOURCES_DIR}/PinViewModel.h
  ${SOURCES_DIR}/WireViewModel.cpp
  ${SOURCES_DIR}/WireViewModel.h
  ${SOURCES_DIR}/CircuitViewModel.cpp
  ${SOURCES_DIR}/CircuitViewModel.h
  ${SOURCES_DIR}/IViewModelListener.h)


target_include_directories(viewmodel_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(viewmodel_lib PRIVATE domain_lib)
