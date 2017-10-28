cmake_minimum_required(VERSION 3.8)
project(BeRealDriver)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

add_executable(BeRealDriver ${SOURCE_FILES})