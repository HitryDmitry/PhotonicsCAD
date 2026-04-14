cmake_minimum_required(VERSION 3.20)
project(editor)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(
  editor_lib SHARED
  ${SOURCES_DIR}/GraphicsComponentItem.cpp
  ${SOURCES_DIR}/GraphicsComponentItem.h ${SOURCES_DIR}/GraphicsView.cpp
  ${SOURCES_DIR}/GraphicsView.h)

target_include_directories(editor_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
  editor_lib PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets
                     component_library)
