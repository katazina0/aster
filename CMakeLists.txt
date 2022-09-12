cmake_minimum_required(VERSION 3.24)
project(aster)

include_directories(include)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall")

add_executable(aster main.cpp)