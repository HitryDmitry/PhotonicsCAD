cmake_minimum_required(VERSION 3.20)
project(domain_lib)

add_subdirectory(simulation)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(
  domain_lib SHARED
  ${SOURCES_DIR}/ComponentInstance.cpp
  ${SOURCES_DIR}/ComponentInstance.h
  ${SOURCES_DIR}/Circuit.h
  ${SOURCES_DIR}/Circuit.cpp
  ${SOURCES_DIR}/PinInstance.cpp
  ${SOURCES_DIR}/PinInstance.h
  ${SOURCES_DIR}/Wire.h
  ${SOURCES_DIR}/Wire.cpp
  ${SOURCES_DIR}/ComponentFactory.h
  ${SOURCES_DIR}/ComponentId.h
  ${SOURCES_DIR}/PinIndex.h
  ${SOURCES_DIR}/ComponentIdGenerator.h
  ${SOURCES_DIR}/PinRef.h)

target_include_directories(domain_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(domain_lib PRIVATE component_library)
