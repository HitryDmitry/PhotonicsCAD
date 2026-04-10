cmake_minimum_required(VERSION 3.20)
project(component_library)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(
  component_library SHARED
  ${SOURCES_DIR}/ComponentLibraryManager.cpp
  ${SOURCES_DIR}/ComponentLibraryManager.h ${SOURCES_DIR}/ComponentDefinition.h)

target_link_libraries(component_library PRIVATE Qt${QT_VERSION_MAJOR}::Core)
