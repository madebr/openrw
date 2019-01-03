set(TESTS
    Animation
    Archive
    Buoyancy
    Character
    Chase
    Config
    Cutscene
    Data
    FileIndex
    GameData
    GameWorld
    Garage
    HitTest
    Input
    Items
    Lifetime
    LoaderDFF
    LoaderIDE
    LoaderIPL
    Logger
    Menu
    Object
    Payphone
    Pickup
    Renderer
    RWBStream
    SaveGame
    ScriptMachine
    State
    StringEncoding
    Sound
    Text
    TrafficDirector
    Vehicle
    VisualFX
    Weapon
    World
    ZoneData
    )

set(TEST_SOURCES
    main.cpp
    test_Globals.cpp
    test_Globals.hpp
    )

foreach(TEST ${TESTS})
    list(APPEND TEST_SOURCES "test_${TEST}.cpp")
endforeach()

if(BUILD_PYTHON)
    set(PYTHON_TEST_MODULES
        test_Logger
        )

    foreach(PYTHON_TEST_MODULE ${PYTHON_TEST_MODULES})
        list(APPEND TEST_SOURCES "${PYTHON_TEST_MODULE}.py")
    endforeach()
endif()

add_executable(rwtests
    ${TEST_SOURCES}
    )

target_compile_definitions(rwtests
    PRIVATE
        "RW_TEST_WITH_DATA=$<NOT:$<BOOL:${TESTS_NODATA}>>"
    )

target_include_directories(rwtests
    PRIVATE
        "${PROJECT_SOURCE_DIR}/tests"
        "${PROJECT_SOURCE_DIR}/rwgame"
    )

target_link_libraries(rwtests
    PRIVATE
        Boost::unit_test_framework
        librwgame
    )

openrw_target_apply_options(
    TARGET rwtests
    CORE
    COVERAGE
    INSTALL INSTALL_PDB
    )

if(SEPARATE_TEST_SUITES)
    foreach(TEST ${TESTS})
        add_test(
            NAME "${TEST}"
            COMMAND "$<TARGET_FILE:rwtests>" "-t" "${TEST}Tests"
            )
        set_tests_properties("${TEST}"
            PROPERTIES
                TIMEOUT 300
            )
    endforeach()
    if(BUILD_PYTHON)
        foreach(PYTHON_TEST_MODULE ${PYTHON_TEST_MODULES})
            add_test(
                NAME "python_${PYTHON_TEST_MODULE}"
                COMMAND "${CMAKE_COMMAND}" -E env "LD_PRELOAD=${LD_PRELOAD}" "ASAN_OPTIONS=exitcode=0" "PYTHONPATH=$<TARGET_FILE_DIR:pyopenrw>" "${PYTHON_EXECUTABLE}" -m unittest "${PYTHON_TEST_MODULE}"
                WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
                )
            set_tests_properties("python_${PYTHON_TEST_MODULE}"
                PROPERTIES
                    TIMEOUT 300
                )
        endforeach()
    endif()
else()
    add_test(NAME UnitTests
        COMMAND "$<TARGET_FILE:rwtests>"
        )
    set_tests_properties(UnitTests
        PROPERTIES
            TIMEOUT 300
        )
    if(BUILD_PYTHON)
        add_test(
            NAME PythonTests
            COMMAND "${CMAKE_COMMAND}" -E env "LD_PRELOAD=${LD_PRELOAD}" "ASAN_OPTIONS=exitcode=0" "PYTHONPATH=$<TARGET_FILE_DIR:pyopenrw>" "${PYTHON_EXECUTABLE}" -m unittest discover
            WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
            )
        set_tests_properties(PythonTests
            PROPERTIES
                TIMEOUT 300
            )
    endif()
endif()
