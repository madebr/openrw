add_library(rw_glew
    src/glew.c)
target_include_directories(rw_glew
    PUBLIC
        include
    )
