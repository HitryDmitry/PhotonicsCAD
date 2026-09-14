cmake_minimum_required(VERSION 3.20)
project(ui)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGLWidgets)

add_subdirectory(imgui)
add_subdirectory(implot)

set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(
  ui_lib SHARED
  ${SOURCES_DIR}/ComponentListWidget.cpp ${SOURCES_DIR}/ComponentListWidget.h
  ${SOURCES_DIR}/PropertyEditorDialog.cpp ${SOURCES_DIR}/PropertyEditorDialog.h
  ${SOURCES_DIR}/ImPlotWidget.cpp ${SOURCES_DIR}/ImPlotWidget.h)

target_include_directories(ui_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
  ui_lib
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets
          Qt${QT_VERSION_MAJOR}::OpenGLWidgets component_library domain_lib
          viewmodel_lib)
