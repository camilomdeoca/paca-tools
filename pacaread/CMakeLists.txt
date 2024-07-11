add_library(pacaread STATIC
    include/pacaread/pacaread.hpp
    pacaread.cpp
)

target_include_directories(pacaread PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/pacaread>
    $<INSTALL_INTERFACE:include/pacaread>
)

target_include_directories(pacaread PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
