cmake_minimum_required(VERSION 3.20)
project(viewmodeltests)

add_library(viewmodeltests_lib OBJECT
            ${CMAKE_CURRENT_SOURCE_DIR}/CircuitViewModelTest.cpp)

# Получаем include-директории из doctest
get_target_property(DOCTEST_INCLUDE_DIRS doctest::doctest
                    INTERFACE_INCLUDE_DIRECTORIES)

target_include_directories(viewmodeltests_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                                     ${DOCTEST_INCLUDE_DIRS})

target_link_libraries(viewmodeltests_lib domain_lib viewmodel_lib
                      component_library test_helpers)
