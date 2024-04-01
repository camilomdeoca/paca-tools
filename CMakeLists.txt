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
target_link_libraries(to-paca-format pacaconvert pacawrite)

# pacaread
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

# pacawrite
add_library(pacawrite STATIC
    pacawrite/include/pacawrite/pacawrite.hpp
    pacawrite/pacawrite.cpp
)

set_target_properties(pacawrite PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build
)

target_include_directories(pacawrite PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pacawrite/include/pacawrite>
    $<INSTALL_INTERFACE:include/pacawrite>
)

target_include_directories(pacawrite PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pacawrite/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(pacawrite pacaread)

# pacaconvert
add_library(pacaconvert STATIC
    pacaconvert/include/pacaconvert/pacaconvert.hpp
    pacaconvert/pacaconvert.cpp
)

set_target_properties(pacaconvert PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build
)

target_include_directories(pacaconvert PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pacaconvert/include/pacaconvert>
    $<INSTALL_INTERFACE:include/pacaconvert>
)

target_include_directories(pacaconvert PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pacaconvert/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(pacaconvert pacaread assimp)

