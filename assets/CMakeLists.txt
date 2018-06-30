find_package(ImageMagick COMPONENTS convert REQUIRED)

function(imagemagick_convert INPUT OUTPUT SIZE)
    add_custom_command(
        OUTPUT "${OUTPUT}"
        COMMAND "${IMAGEMAGICK_CONVERT_EXECUTABLE}" +antialias -background transparent "${INPUT}" -resize "${SIZE}" "${OUTPUT}"
        DEPENDS "${INPUT}"
    )
endfunction()
