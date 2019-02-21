if(NOT EXISTS glew/src/glew.c)
    message(STATUS "Regenerating glew...")
    file(REMOVE_RECURSE glew/auto/OpenGL-Registry)
    file(REMOVE_RECURSE glew/auto/glfixes)
    file(REMOVE_RECURSE glew/auto/EGL-Registry)
    execute_process(
        COMMAND make -C "${CMAKE_CURRENT_LIST_DIR}/glew" extensions
        RESULT_VARIABLE _GLEW_RES
        )
    if(NOT _GLEW_RES EQUAL "0")
        file(REMOVE glew/src/glew.c)
        message(FATAL_ERROR "GLEW: generating extensions failed")
    endif()
endif()

add_subdirectory(glew/build/cmake)

add_library(rw_glew INTERFACE)
target_link_libraries(rw_glew
    INTERFACE
        glew_s)
target_include_directories(rw_glew
    INTERFACE
        glew/include
    )
