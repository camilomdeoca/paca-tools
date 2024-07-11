file(GLOB_RECURSE TO_PACA_FORMAT_SOURCES *.cpp )
add_executable(to-paca-format ${TO_PACA_FORMAT_SOURCES})
target_link_libraries(to-paca-format pacaconvert pacawrite)
