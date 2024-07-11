# Serializer generator
find_package(Clang REQUIRED)
set(LLVM_LINK_COMPONENTS support)
add_executable(serializer-generator
    Main.cpp
    ASTParser.cpp
    CodeGenerator.cpp
    SerializerTemplate.cpp
)
set_target_properties(serializer-generator PROPERTIES
    CXX_STANDARD 17
    CXX_COMPILER /usr/bin/clang++
)
target_include_directories(serializer-generator
    PRIVATE
    /usr/include
    ${CLANG_INCLUDE_DIRS}
)
target_link_libraries(serializer-generator
    PRIVATE
    clang-cpp
    LLVM-17
)

# Generate serializer template before build
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_SOURCE_DIR}/SerializerTemplate.cpp
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/generate-serializer-template.sh ${CMAKE_CURRENT_SOURCE_DIR}/SerializerTemplate.cpp
    DEPENDS
        generate-serializer-template.sh
        Serializer.hpp.template
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(gen_serializer_template_header
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/SerializerTemplate.cpp
)

add_dependencies(serializer-generator
    gen_serializer_template_header
)

# test
#file(GLOB_RECURSE SERIALIZER_GENERATOR_TEST_SOURCES test/*.cpp )
set(SERIALIZER_GENERATOR_TEST_SOURCES
    test/test.cpp
)
add_executable(serializer-generator-test
    ${SERIALIZER_GENERATOR_TEST_SOURCES}
    test/Serializer.cpp
)
set_target_properties(serializer-generator-test PROPERTIES
    #EXCLUDE_FROM_ALL true
    EXPORT_COMPILE_COMMANDS ON
)
add_custom_command(
    OUTPUT
        test/Serializer.hpp
        test/Serializer.cpp
        COMMAND $<TARGET_FILE:serializer-generator> ${PROJECT_SOURCE_DIR} test ${SERIALIZER_GENERATOR_TEST_SOURCES}
    DEPENDS $<TARGET_FILE:serializer-generator>
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(gen-serializer
    DEPENDS
        test/Serializer.hpp
        test/Serializer.cpp
)
add_dependencies(serializer-generator-test
    gen-serializer
)

enable_testing()
add_test(
    NAME serializer-generator-test
    COMMAND $<TARGET_FILE:serializer-generator-test>
)
