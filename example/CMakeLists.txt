cmake_minimum_required(VERSION 3.13)
project(example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(../include)

add_executable(example example.cpp)
