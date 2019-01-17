configure_file("${CMAKE_CURRENT_SOURCE_DIR}/GitSHA1.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/GitSHA1.cpp" @ONLY)

add_library(librwgame STATIC
    GitSHA1.h
    "${CMAKE_CURRENT_BINARY_DIR}/GitSHA1.cpp"

    RWConfig.inc
    RWConfig.hpp
    RWConfig.cpp

    GameBase.hpp
    GameBase.cpp
    RWGame.hpp
    RWGame.cpp
    GameWindow.hpp
    GameWindow.cpp

    HUDDrawer.hpp
    HUDDrawer.cpp
    MenuSystem.hpp
    MenuSystem.cpp
    GameInput.hpp
    GameInput.cpp

    game.hpp
    WindowIcon.hpp

    StateManager.hpp
    StateManager.cpp
    State.hpp
    State.cpp

    states/LoadingState.hpp
    states/LoadingState.cpp
    states/IngameState.hpp
    states/IngameState.cpp
    states/PauseState.hpp
    states/PauseState.cpp
    states/MenuState.hpp
    states/MenuState.cpp
    states/DebugState.hpp
    states/DebugState.cpp
    states/BenchmarkState.hpp
    states/BenchmarkState.cpp
    )

if(BUILD_PYTHON)
    target_link_libraries(librwgame
        PUBLIC
            pyopenrw_private
        )

    target_compile_definitions(librwgame
        PUBLIC
            RW_PYTHON
        )
endif()

openrw_target_apply_options(
    TARGET librwgame
    CORE
    COVERAGE
    )

set_target_properties(librwgame
    PROPERTIES
        OUTPUT_NAME "rwgame"
        PREFIX "lib"
    )

target_include_directories(librwgame
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
    )

target_link_libraries(librwgame
    PUBLIC
        openrw::interface
        rwengine
        Boost::program_options
        SDL2::SDL2
    )

if(ENABLE_IMGUI)
    target_compile_definitions(librwgame
        PUBLIC
            RW_IMGUI
        )

    target_sources(librwgame
        PRIVATE
            RWImGui.cpp
            RWImGui.hpp
            RWRingBuffer.hpp
        )

    target_link_libraries(librwgame
        PUBLIC
            imgui::sdl_gl3
        )
endif()

add_executable(rwgame
    main.cpp
    )

target_link_libraries(rwgame
    PRIVATE
        librwgame
    )

if(BUILD_PYTHON)
    target_sources(rwgame
        PRIVATE
            python/pyembed.cpp
        )

    add_library(pyopenrw_private STATIC
        python/pyopenrw_priv.hpp
        python/pyopenrw.hpp
        python/module.cpp
        python/logging.cpp
        python/config.cpp
        python/game.cpp
        )
    target_link_libraries(pyopenrw_private
        PUBLIC
            pybind11::module
            python::libs
            librwgame
        )
    target_include_directories(pyopenrw_private
        INTERFACE
            "${CMAKE_CURRENT_LIST_DIR}/python"
        )
    openrw_target_apply_options(
        TARGET pyopenrw_private
        CORE
        COVERAGE
        )
    add_library(pyopenrw MODULE
        python/pymain.cpp
        )
    target_link_libraries(pyopenrw
        PUBLIC
            pyopenrw_private
        )
    set_target_properties(pyopenrw
        PROPERTIES
            OUTPUT_NAME "openrw"
            PREFIX "${PYTHON_MODULE_PREFIX}"
            SUFFIX "${PYTHON_MODULE_EXTENSION}"
        )

    openrw_target_apply_options(
        TARGET pyopenrw
        CORE
        COVERAGE
        INSTALL INSTALL_PDB
        )
endif()

openrw_target_apply_options(
    TARGET rwgame
    CORE
    COVERAGE
    INSTALL INSTALL_PDB
    )
