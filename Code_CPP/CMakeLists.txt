cmake_minimum_required(VERSION 3.01)  # CMake version check
project(Code_CPP)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 14)            # Enable c++14 standard

# Add main.cpp file of project root directory as source file
#set(SOURCE_FILES agents.cpp network_dynamics.cpp ruta_facil.cpp)
include_directories(".")

add_definitions(${PCL_DEFINITIONS})
#file(GLOB SOURCE_FILES "./*.cpp")

aux_source_directory(. SRC_LIST)
add_executable(easy_route.x ${SRC_LIST})
