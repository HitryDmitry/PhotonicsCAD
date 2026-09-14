cmake_minimum_required(VERSION 3.20)
project(implot CXX)

# Define the source files
set(IMPLOT_SOURCES implot.h implot.cpp implot_items.cpp)

# Create the static library target
add_library(implot STATIC ${IMPLOT_SOURCES})

# Configure includes (PUBLIC makes these available to anything linking implot)
target_include_directories(implot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link against ImGui (which gives ImPlot access to imgui.h)
target_link_libraries(implot PUBLIC imgui)
