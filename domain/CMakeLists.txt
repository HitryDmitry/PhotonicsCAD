cmake_minimum_required(VERSION 3.20)
project(domain_lib)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(
  domain_lib SHARED
  ${SOURCES_DIR}/ComponentInstance.cpp ${SOURCES_DIR}/ComponentInstance.h
  ${SOURCES_DIR}/Circuit.h ${SOURCES_DIR}/Circuit.cpp)

target_include_directories(domain_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
  domain_lib PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets
                     component_library)
