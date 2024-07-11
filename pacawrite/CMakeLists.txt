add_library(pacawrite STATIC
    include/pacawrite/pacawrite.hpp
    pacawrite.cpp
)

target_include_directories(pacawrite PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/pacawrite>
    $<INSTALL_INTERFACE:include/pacawrite>
)

target_include_directories(pacawrite PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(pacawrite pacaread)

