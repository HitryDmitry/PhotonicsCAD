cmake_minimum_required(VERSION 3.20)
project(component_library)

find_package(Qt6 REQUIRED COMPONENTS Core)

set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(
  component_library SHARED
  ${SOURCES_DIR}/ComponentLibraryManager.cpp
  ${SOURCES_DIR}/ComponentLibraryManager.h ${SOURCES_DIR}/ComponentDefinition.h)

target_include_directories(component_library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(component_library PUBLIC Qt${QT_VERSION_MAJOR}::Core)
