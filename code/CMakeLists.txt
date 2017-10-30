cmake_minimum_required(VERSION 3.8)
project(BeRealDriver)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp config.hpp lane_finder.cpp lane_finder.hpp)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

find_package(OpenCV REQUIRED)

add_executable(BeRealDriver ${SOURCE_FILES} array_tools.hpp)

target_link_libraries(BeRealDriver ${OpenCV_LIBS})
