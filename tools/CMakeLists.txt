find_package(Boost COMPONENTS program_options REQUIRED)

add_executable(rwembedder EXCLUDE_FROM_ALL
    rwembedder.cpp
    )

target_include_directories(rwembedder
    PRIVATE
        ${Boost_INCLUDE_DIRS}
    )

target_link_libraries(rwembedder
    PRIVATE
        rwlib
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
    )

include(CMakeParseArguments)
function(rwembedder_generate_file)
    cmake_parse_arguments("RWEMBED" "" "INPUT;SOURCE;HEADER;NAME" "" ${ARGN})
    if(NOT RWEMBED_INPUT)
        message(FATAL_ERROR "rwembedder_generate_file needs input file name")
    endif()
    if(NOT RWEMBED_SOURCE)
        message(FATAL_ERROR "rwembedder_generate_file needs output source file name")
    endif()
    set(RWEMBEDDER_ARGS "$<TARGET_FILE:rwembedder>" -i "${RWEMBED_INPUT}" -s "${RWEMBED_SOURCE}")
    set(OUTPUTFILES "${RWEMBED_SOURCE}")
    if(RWEMBED_HEADER)
        list(APPEND RWEMBEDDER_ARGS -h "${RWEMBED_HEADER}")
        list(APPEND OUTPUTFILES "${RWEMBED_HEADER}")
    endif()
    if(RWEMBED_NAME)
        list(APPEND RWEMBEDDER_ARGS -n "${RWEMBED_NAME}")
    endif()
    add_custom_command(
        OUTPUT ${OUTPUTFILES}
        COMMAND ${RWEMBEDDER_ARGS}
        DEPENDS "${RWEMBED_INPUT}" rwembedder
        )
endfunction()
