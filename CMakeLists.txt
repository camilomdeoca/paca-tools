cmake_minimum_required(VERSION 3.27)
project(paca-tools)

# Set C++ standard and optimization level
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG -Wall -Wpedantic")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

enable_testing()

add_subdirectory(to-paca-format)

add_subdirectory(pacaread)
add_subdirectory(pacawrite)

add_subdirectory(pacaconvert)

add_subdirectory(serializer-generator)
