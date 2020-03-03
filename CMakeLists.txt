cmake_minimum_required(VERSION 3.01)  # CMake version check
project(Easy_route)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 14)            # Enable c++14 standard

# Add main.cpp file of project root directory as source file
#set(SOURCE_FILES main.cpp)
include_directories(".")

add_definitions(${PCL_DEFINITIONS})

aux_source_directory(. SRC_LIST)
add_executable(easy_route.x ${SRC_LIST})
