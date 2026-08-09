cmake_minimum_required(VERSION 3.20)
project(test_helpers)

# This target does nothing for the build, but forces Qt Creator to display the
# file
add_custom_target(helpers_ide_view SOURCES TestHelpers.h)

add_library(test_helpers INTERFACE)
target_include_directories(test_helpers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
