function(add_stb_library NAME)
    add_library("stb_${NAME}"
        EXCLUDE_FROM_ALL
        ${ARGN}
        )

    target_include_directories("stb_${NAME}"
        PUBLIC
            stb
        )

    add_library("stb::${NAME}" ALIAS "stb_${NAME}")
endfunction()

add_stb_library(image
    stb/stb_image.h
    stb_image.cpp
    )

add_stb_library(image_resize
    stb/stb_image_resize.h
    stb_image_resize.cpp
    )

add_stb_library(image_write
    stb/stb_image_write.h
    stb_image_write.cpp
    )

add_stb_library(truetype
    stb/stb_truetype.h
    stb_truetype.cpp
    )
