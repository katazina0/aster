cmake_minimum_required(VERSION 3.24)
include_directories(include)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wno-implicit-fallthrough -Wno-switch -Wall -Wextra -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(/home/kata/projects/aster/include)

project(aster)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../out)
add_executable(aster main.cpp)
set_target_properties(aster PROPERTIES OUTPUT_NAME aster.o)