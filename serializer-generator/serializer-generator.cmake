function(add_serializer_generation TARGET)
    cmake_parse_arguments(
        ARG
        ""
        "COMPILATION_DB;HEADER;CODE"
        "FILES"
        ${ARGN}
    )

    #message("TARGET: ${TARGET}")
    #message("HEADER: ${ARG_HEADER}")
    #message("CODE: ${ARG_CODE}")
    #message("FILES: ${ARG_FILES}")

    if (NOT DEFINED ARG_COMPILATION_DB)
        set(ARG_COMPILATION_DB ${PROJECT_SOURCE_DIR})
    endif()

    add_custom_command(
        OUTPUT
            ${ARG_HEADER}
            ${ARG_CODE}
        COMMAND $<TARGET_FILE:serializer-generator> --compilation-db ${ARG_COMPILATION_DB} --out-header ${ARG_HEADER} --out-code ${ARG_CODE} ${ARG_FILES}
        DEPENDS $<TARGET_FILE:serializer-generator>
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    add_custom_target(gen-serializer
        DEPENDS
            ${ARG_HEADER}
            ${ARG_CODE}
    )
    add_dependencies(${TARGET}
        gen-serializer
    )
endfunction()
