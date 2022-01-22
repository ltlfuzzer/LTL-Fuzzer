cmake_minimum_required(VERSION 3.10)

###set the project name ###
set(This afl-ltl)
project($(This) C CXX)

### specify the C++ standard ###
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_VERBOSE_MAKEFILE ON)

add_compile_options(-Wall -g -MMD -MP -pedantic)

###headers###
include_directories(${CMAKE_SOURCE_DIR}/include)

### source ###
add_subdirectory(src)