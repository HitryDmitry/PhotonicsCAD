cmake_minimum_required(VERSION 3.20)
project(imgui CXX)

# Gather core ImGui files
set(IMGUI_SOURCES
    imgui.h
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui_demo.cpp
    backends/imgui_impl_opengl3.h
    backends/imgui_impl_opengl3.cpp)

# Create the static library target
add_library(imgui STATIC ${IMGUI_SOURCES})

# Configure include directories
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                        ${CMAKE_CURRENT_SOURCE_DIR}/backends)
