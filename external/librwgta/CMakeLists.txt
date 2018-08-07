cmake_minimum_required(VERSION 3.8)

project(librwgta C CXX)

set(LIBRWGTA_DIR "${PROJECT_SOURCE_DIR}/librwgta")

option(LIBRWGTA_ENABLE_CHECKS "Enable compile time checks on librw" OFF)

function(librwgta_add_library TARGET LIBRW_TARGET)
    add_library("${TARGET}"
        "${LIBRWGTA_DIR}/src/collisions.cpp"
        "${LIBRWGTA_DIR}/src/collisions.h"
        "${LIBRWGTA_DIR}/src/gtaplg.cpp"
        "${LIBRWGTA_DIR}/src/rsps2.cpp"
        "${LIBRWGTA_DIR}/src/rsxbox.cpp"
        "${LIBRWGTA_DIR}/src/rwgta.h"
        )

    target_include_directories("${TARGET}" SYSTEM
        INTERFACE
            "${LIBRWGTA_DIR}/src"
        )

    target_link_libraries("${TARGET}"
        PUBLIC
            "${LIBRW_TARGET}"
            openrw::checks
        )

    if(LIBRWGTA_ENABLE_CHECKS)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
            target_compile_options("${TARGET}"
                PRIVATE
                    "-Wall"
                    "-Wextra"
                    "-Wdouble-promotion"
                    "-Wpedantic"
                )
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            target_compile_options("${TARGET}"
                INTERFACE
                    /wd4996 /wd4244
                )
        endif()
    endif()

    set_target_properties("${TARGET}"
        PROPERTIES
            C_STANDDARD 99
            C_EXTENSIONS OFF
            C_STANDARD_REQUIRED ON
            CXX_STANDARD 98
            CXX_EXTENSIONS OFF
            CXX_STANDARD_REQUIRED ON
            PREFIX ""
        )
endfunction()

librwgta_add_library(librwgta_null librw_null)
librwgta_add_library(librwgta_gl3 librw_gl3)
