cmake_minimum_required(VERSION 3.27)
project(paca-tools)

# Set C++ standard and optimization level
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG -Wall -Wpedantic")

# Set build path
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)

# To paca format converter
file(GLOB_RECURSE TO_PACA_FORMAT_SOURCES to-paca-format/*.cpp )
add_executable(to-paca-format ${TO_PACA_FORMAT_SOURCES})
target_link_libraries(to-paca-format
    glm
    assimp
)

# Paca format reader lib
add_library(pacaread STATIC
    pacaread/include/pacaread/pacaread.hpp
    pacaread/pacaread.cpp
)

set_target_properties(pacaread PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build
)

target_include_directories(pacaread PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pacaread/include/pacaread>
    $<INSTALL_INTERFACE:include/pacaread>
)

target_include_directories(pacaread PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pacaread/include>
    $<INSTALL_INTERFACE:include>
)

