find_package(yaml-cpp REQUIRED)

add_executable(rwtxddump
    rwtxddump.cpp
    names.hpp
    names.cpp

    librw/streams.hpp
    librw/streams.cpp
    #RWBCreate.cpp
    )

target_link_libraries(rwtxddump
    PUBLIC
        rwcore
        librw_null
        librwgta_null

        yaml-cpp
        Boost::program_options
    )

openrw_target_apply_options(
    TARGET rwtxddump
    )

install(TARGETS rwtxddump
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
