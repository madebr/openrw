find_program(GLAD_BIN
    NAMES glad
    HINTS
        "$ENV{HOME}/.local/bin"
    )

if(NOT GLAD_BIN)
    message(FATAL_ERROR "Cannot find glad program.")
endif()

function(glad_add_library TARGET)
    cmake_parse_arguments("GG" "DEBUG;NOLOADER;OMIT_KHRPLATFORM" "LOCATION;SPEC;PROFILE" "EXTENSIONS;API" ${ARGN})

    set(GLAD_DIR "${PROJECT_BINARY_DIR}/generated_${TARGET}")
    if (GG_LOCATION)
        set(GLAD_DIR "${GG_LOCATION}")
    endif()
    if(NOT IS_DIRECTORY "${GLAD_DIR}")
        file(MAKE_DIRECTORY "${GLAD_DIRECTORY}")
    endif()
#    set(GLAD_ARGFILE "${GLAD_DIR}/args.txt")

    set(GLAD_ARGS --out-path "${GLAD_DIR}")

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

    # regenerate files when argument changes
    add_custom_command(OUTPUT ${GLAD_FILES}
        COMMAND "${CMAKE_COMMAND}" -E remove_directory "${GLAD_DIR}"
        COMMAND ${GLAD_BIN} ${GLAD_ARGS}
        )

    # add make custom target
    add_custom_target("regenerate_${TARGET}"
        COMMAND "${CMAKE_COMMAND}" -E remove_directory "${GLAD_DIR}"
        COMMAND ${GLAD_BIN} ${GLAD_ARGS}
        COMMENT "Regenerating glad source files for ${TARGET}..."
        )

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
