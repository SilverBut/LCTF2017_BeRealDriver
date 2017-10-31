cmake_minimum_required(VERSION 3.8)
project(BeRealDriver)

set(CMAKE_CXX_STANDARD 17)

# Copy some scripts from
# https://beesbuzz.biz/blog/e/2014/07/31-embedding_binary_resources_with_cmake_and_c11.php
# Compilation step for static resources
FUNCTION(ADD_RESOURCES out_var)
    SET(result)
    FOREACH (in_f ${ARGN})
        FILE(RELATIVE_PATH src_f ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${in_f})
        SET(out_f "${PROJECT_BINARY_DIR}/${in_f}.o")
        ADD_CUSTOM_COMMAND(OUTPUT ${out_f}
                COMMAND ld -r -b binary -o ${out_f} ${src_f}
                DEPENDS ${in_f}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Building GLSL object ${out_f}"
                VERBATIM
                )
        LIST(APPEND result ${out_f})
    ENDFOREACH ()
    SET(${out_var} "${result}" PARENT_SCOPE)
ENDFUNCTION()


set(SOURCE_FILES main.cpp config.hpp lane_finder.cpp lane_finder.hpp Resource.hpp)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif ()

find_package(OpenCV REQUIRED)

ADD_RESOURCES(MapSourceRes road.jpg)
ADD_RESOURCES(OpSourceRes op.jpg)

add_executable(BeRealDriver ${SOURCE_FILES} array_tools.hpp ${MapSourceRes} ${OpSourceRes})

target_link_libraries(BeRealDriver ${OpenCV_LIBS})
