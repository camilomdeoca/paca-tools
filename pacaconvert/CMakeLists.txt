add_library(pacaconvert STATIC
    include/pacaconvert/pacaconvert.hpp
    pacaconvert.cpp
)

target_include_directories(pacaconvert PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/pacaconvert>
    $<INSTALL_INTERFACE:include/pacaconvert>
)

target_include_directories(pacaconvert PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(pacaconvert pacaread assimp)

