cmake_minimum_required(VERSION 3.20)
project(PhotonicsCAD)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(app)
add_subdirectory(domain)
add_subdirectory(plugins/components)
add_subdirectory(view)
add_subdirectory(viewmodel)
add_subdirectory(serialization)
add_subdirectory(tests)