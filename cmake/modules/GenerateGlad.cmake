find_program(GLAD_BIN
    NAMES glad
    HINTS
        "$ENV{HOME}/.local/bin"
    )

function(glad_add_library TARGET)
    cmake_parse_arguments("GG" "DEBUG;NOLOADER;OMIT_KHRPLATFORM" "LOCATION;SPEC;PROFILE" "EXTENSIONS;API" ${ARGN})

    set(GLAD_DIR "${PROJECT_BINARY_DIR}/generated_${TARGET}")
    if (GG_LOCATION)
        set(GLAD_DIR "${GG_LOCATION}")
    endif()
    if(NOT IS_DIRECTORY "${GLAD_DIR}")
        file(MAKE_DIRECTORY "${GLAD_DIR}")
    endif()
    set(GLAD_ARGFILE "${GLAD_DIR}/args.txt")

    set(GLAD_ARGS --reproducible --out-path ".")

    set(GLAD_GENERATOR "c")
    if(GG_DEBUG)
        set(GLAD_GENERATOR "c-debug")
    endif()
    list(APPEND GLAD_ARGS --generator "${GLAD_GENERATOR}")

    if(GG_PROFILE)
        set(GLAD_PROFILE "${GG_PROFILE}")
        list(APPEND GLAD_ARGS --profile "${GLAD_PROFILE}")
    endif()

    if(GG_OMIT_KHRPLATFORM)
        list(APPEND GLAD_ARGS --omit-khrplatform)
    endif()

    set(GLAD_SPEC "gl")
    if(GG_SPEC)
        set(GLAD_SPEC "${GG_SPEC}")
    endif()
    list(APPEND GLAD_ARGS --spec "${GLAD_SPEC}")

    if(GG_NOLOADER)
        list(APPEND GLAD_ARGS --no-loader)
    endif()

    if(GG_API)
        list(APPEND GLAD_ARGS "--api;${GG_API}")
    endif()

    set(GLAD_FILES
        "${GLAD_DIR}/include/glad/glad.h"
        )
    if(NOT GG_OMIT_KHRPLATFORM)
        list(APPEND GLAD_FILES "${GLAD_DIR}/include/KHR/khrplatform.h")
    endif()
    if(GG_SPEC MATCHES "gl")
        list(APPEND GLAD_FILES "${GLAD_DIR}/src/glad.c")
    elseif(GG_SPEC MATCHES "egl")
        list(APPEND GLAD_FILES "${GLAD_DIR}/src/glad_egl.c")
    elseif(GG_SPEC MATCHES "glx")
        list(APPEND GLAD_FILES "${GLAD_DIR}/src/glad_glx.c")
    elseif(GG_SPEC MATCHES "wgl")
        list(APPEND GLAD_FILES "${GLAD_DIR}/src/glad_wgl.c")
    endif()

    set(GLAD_OUT_OF_DATE OFF)
    if(EXISTS "${GLAD_ARGFILE}")
        file(READ "${GLAD_ARGFILE}" CURRENT_ARGS)
        string(STRIP "${CURRENT_ARGS}" CURRENT_ARGS)
        if(NOT(CURRENT_ARGS STREQUAL GLAD_ARGS))
            message(STATUS "Current glad sources have been generated by different arguments... glad out of date")
            set(GLAD_OUT_OF_DATE ON)
            file(REMOVE "${GLAD_ARGFILE}")
        endif()
    else()
        message(STATUS "Glad argument file not found... glad out of date")
        set(GLAD_OUT_OF_DATE ON)
    endif()

    if(GLAD_OUT_OF_DATE)
        message(STATUS "glad out of date: adding glad generator do dependency tree")
        if(NOT GLAD_BIN)
            message(FATAL_ERROR "glad not found.")
        endif()
        # regenerate files when argument changes
        add_custom_command(OUTPUT ${GLAD_FILES} "${GLAD_ARGFILE}"
            COMMAND "${CMAKE_COMMAND}" -E remove_directory "${GLAD_DIR}/include"
            COMMAND "${CMAKE_COMMAND}" -E remove_directory "${GLAD_DIR}/src"
            COMMAND ${GLAD_BIN} ${GLAD_ARGS}
            COMMAND "${CMAKE_COMMAND}" -E echo "${GLAD_ARGS}" > "${GLAD_ARGFILE}"
            WORKING_DIRECTORY "${GLAD_DIR}"
            VERBATIM
            )

        # add make custom target
        add_custom_target("regenerate_${TARGET}"
            COMMAND "${CMAKE_COMMAND}" -E remove_directory "${GLAD_DIR}/include"
            COMMAND "${CMAKE_COMMAND}" -E remove_directory "${GLAD_DIR}/src"
            COMMAND ${GLAD_BIN} ${GLAD_ARGS}
            COMMAND "${CMAKE_COMMAND}" -E echo "${GLAD_ARGS}" > "${GLAD_ARGFILE}"
            COMMENT "Regenerating glad source files for ${TARGET}..."
            WORKING_DIRECTORY "${GLAD_DIR}"
            VERBATIM
            )
    endif()

    add_library("${TARGET}" ${GLAD_FILES}
        )

    target_include_directories("${TARGET}"
        PUBLIC
            "${GLAD_DIR}/include"
        )

    target_link_libraries("${TARGET}"
        PUBLIC
            ${CMAKE_DL_LIBS}
        )
endfunction()
