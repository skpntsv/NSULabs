cmake_minimum_required(VERSION 3.0)

project(BigInt)

set(SOURCES BigInt.cpp test.cpp)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/58d77fa8070e8cec2dc1ed015d66b454c8d78850.zip
)
FetchContent_MakeAvailable(googletest)

 
# Link runTests with what we want to test and the GTest and pthread library

add_executable(runTests ${SOURCES})
target_link_libraries(runTests gtest gtest_main pthread)
