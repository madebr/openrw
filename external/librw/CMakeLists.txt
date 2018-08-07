cmake_minimum_required(VERSION 3.8)

project(librw C CXX)

set(LIBRW_DIR "${PROJECT_SOURCE_DIR}/librw")

include(CMakeParseArguments)

option(LIBRW_ENABLE_CHECKS "Enable compile time checks on librw" OFF)

function(librw_add_library TARGET)
    set(_PLATFORMS "NULL;D3D9;GL3;PS2")
    cmake_parse_arguments("LRW" "" "" "PLATFORMS" ${ARGN})

    if(GL3 IN_LIST LRW_PLATFORMS)
        # FIXME: use one loader (glLoadGen, glad, ...)
        find_package(GLEW REQUIRED)
    endif()

    add_library("${TARGET}"
        "${LIBRW_DIR}/args.h"
        "${LIBRW_DIR}/rw.h"

        "${LIBRW_DIR}/src/base.err"
        "${LIBRW_DIR}/src/anim.cpp"
        "${LIBRW_DIR}/src/base.cpp"
        "${LIBRW_DIR}/src/bmp.cpp"
        "${LIBRW_DIR}/src/camera.cpp"
        "${LIBRW_DIR}/src/clump.cpp"
        "${LIBRW_DIR}/src/engine.cpp"
        "${LIBRW_DIR}/src/error.cpp"
        "${LIBRW_DIR}/src/frame.cpp"
        "${LIBRW_DIR}/src/geometry.cpp"
        "${LIBRW_DIR}/src/geoplg.cpp"
        "${LIBRW_DIR}/src/hanim.cpp"
        "${LIBRW_DIR}/src/image.cpp"
        "${LIBRW_DIR}/src/light.cpp"
        "${LIBRW_DIR}/src/matfx.cpp"
        "${LIBRW_DIR}/src/pipeline.cpp"
        "${LIBRW_DIR}/src/plg.cpp"
        "${LIBRW_DIR}/src/prim.cpp"
        "${LIBRW_DIR}/src/render.cpp"
        "${LIBRW_DIR}/src/rwanim.h"
        "${LIBRW_DIR}/src/rwengine.h"
        "${LIBRW_DIR}/src/rwerror.h"
        "${LIBRW_DIR}/src/rwobjects.h"
        "${LIBRW_DIR}/src/rwpipeline.h"
        "${LIBRW_DIR}/src/rwplg.h"
        "${LIBRW_DIR}/src/rwplugins.h"
        "${LIBRW_DIR}/src/rwrender.h"
        "${LIBRW_DIR}/src/rwuserdata.h"
        "${LIBRW_DIR}/src/skin.cpp"
        "${LIBRW_DIR}/src/texture.cpp"
        "${LIBRW_DIR}/src/tga.cpp"
        "${LIBRW_DIR}/src/tristrip.cpp"
        "${LIBRW_DIR}/src/userdata.cpp"
        "${LIBRW_DIR}/src/uvanim.cpp"
        "${LIBRW_DIR}/src/world.cpp"

        "${LIBRW_DIR}/src/d3d/d3d8.cpp"
        "${LIBRW_DIR}/src/d3d/d3d8matfx.cpp"
        "${LIBRW_DIR}/src/d3d/d3d8render.cpp"
        "${LIBRW_DIR}/src/d3d/d3d8skin.cpp"
        "${LIBRW_DIR}/src/d3d/d3d9.cpp"
        "${LIBRW_DIR}/src/d3d/d3d9matfx.cpp"
        "${LIBRW_DIR}/src/d3d/d3d9render.cpp"
        "${LIBRW_DIR}/src/d3d/d3d9skin.cpp"
        "${LIBRW_DIR}/src/d3d/d3d.cpp"
        "${LIBRW_DIR}/src/d3d/d3ddevice.cpp"
        "${LIBRW_DIR}/src/d3d/d3dimmed.cpp"
        "${LIBRW_DIR}/src/d3d/d3drender.cpp"
        "${LIBRW_DIR}/src/d3d/rwd3d8.h"
        "${LIBRW_DIR}/src/d3d/rwd3d9.h"
        "${LIBRW_DIR}/src/d3d/rwd3d.h"
        "${LIBRW_DIR}/src/d3d/rwd3dimpl.h"
        "${LIBRW_DIR}/src/d3d/rwxbox.h"
        "${LIBRW_DIR}/src/d3d/rwxboximpl.h"
        "${LIBRW_DIR}/src/d3d/xbox.cpp"
        "${LIBRW_DIR}/src/d3d/xboxmatx.cpp"
        "${LIBRW_DIR}/src/d3d/xboxskin.cpp"
        "${LIBRW_DIR}/src/d3d/xboxvfmt.cpp"

        "${LIBRW_DIR}/src/gl/gl3.cpp"
        "${LIBRW_DIR}/src/gl/gl3device.cpp"
        "${LIBRW_DIR}/src/gl/gl3immed.cpp"
        "${LIBRW_DIR}/src/gl/gl3matfx.cpp"
        "${LIBRW_DIR}/src/gl/gl3pipe.cpp"
        "${LIBRW_DIR}/src/gl/gl3raster.cpp"
        "${LIBRW_DIR}/src/gl/gl3render.cpp"
        "${LIBRW_DIR}/src/gl/gl3shader.cpp"
        "${LIBRW_DIR}/src/gl/gl3skin.cpp"
        "${LIBRW_DIR}/src/gl/rwgl3.h"
        "${LIBRW_DIR}/src/gl/rwgl3impl.h"
        "${LIBRW_DIR}/src/gl/rwgl3plg.h"
        "${LIBRW_DIR}/src/gl/rwgl3shader.h"
        "${LIBRW_DIR}/src/gl/rwwdgl.h"
        "${LIBRW_DIR}/src/gl/wdgl.cpp"

        "${LIBRW_DIR}/src/ps2/pds.cpp"
        "${LIBRW_DIR}/src/ps2/ps2.cpp"
        "${LIBRW_DIR}/src/ps2/ps2device.cpp"
        "${LIBRW_DIR}/src/ps2/ps2matfx.cpp"
        "${LIBRW_DIR}/src/ps2/ps2raster.cpp"
        "${LIBRW_DIR}/src/ps2/ps2skin.cpp"
        "${LIBRW_DIR}/src/ps2/rwps2.h"
        "${LIBRW_DIR}/src/ps2/rwps2impl.h"
        "${LIBRW_DIR}/src/ps2/rwps2plg.h"
        )

    target_include_directories("${TARGET}"
        PRIVATE
            "${LIBRW_DIR}"
        )

    target_include_directories("${TARGET}" SYSTEM
        INTERFACE
            "${LIBRW_DIR}"
        )

    target_link_libraries("${TARGET}"
        PUBLIC
            openrw::checks
        )

    target_compile_definitions("${TARGET}"
        PRIVATE
            "$<IF:$<CONFIG:DEBUG>,DEBUG,NDEBUG>"
        )

    foreach(LIBRW_PLATFORM ${LRW_PLATFORMS})
        target_compile_definitions("${TARGET}"
            PUBLIC
                "RW_${LIBRW_PLATFORM}"
        )
    endforeach()

    if(LIBRW_ENABLE_CHECKS)
        message("ENABLE CHECKS")
        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
            target_compile_options("${TARGET}"
                PRIVATE
                    "-Wall"
                    "-Wno-variadic-macros"
                    "-Wno-unused-parameter"
                    "-Wextra"
                    "-Wdouble-promotion"
                    "-Wpedantic"
                )
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            target_compile_options("${TARGET}"
                PUBLIC
                    /wd4996 /wd4244
                )
        endif()
    endif()

    set_target_properties("${TARGET}"
        PROPERTIES
            C_STANDARD 99
            C_EXTENSIONS OFF
            C_STANDARD_REQUIRED ON
            CXX_STANDARD 98
            CXX_EXTENSIONS OFF
            CXX_STANDARD_REQUIRED ON
            PREFIX ""
        )

    if(GL3 IN_LIST LRW_PLATFORMS)
        target_link_libraries("${TARGET}"
            PUBLIC
                GLEW::GLEW
                OpenGL::GL
                SDL2::SDL2
            )
        target_compile_definitions("${TARGET}"
            PUBLIC
                "LIBRW_SDL2"
            )
    endif()

    if(D3D9 IN_LIST LRW_PLATFORMS)
        target_link_libraries("${TARGET}"
            PUBLIC
                d3d9
                xinput
            )
    endif()
endfunction()

librw_add_library(librw_null
    PLATFORMS NULL
    )

librw_add_library(librw_gl3
    PLATFORMS GL3
    )
