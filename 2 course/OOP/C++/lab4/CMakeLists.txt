cmake_minimum_required(VERSION 3.6)
project(lab4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES main.cpp)


add_executable(run ${SOURCE_FILES})