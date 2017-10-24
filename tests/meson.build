rwtests_sources = files(
    'main.cpp',
    'test_globals.cpp',
    'test_animation.cpp',
    'test_archive.cpp',
    'test_buoyancy.cpp',
    'test_character.cpp',
    'test_chase.cpp',
    'test_cutscene.cpp',
    'test_config.cpp',
    'test_data.cpp',
    'test_FileIndex.cpp',
    'test_GameData.cpp',
    'test_GameWorld.cpp',
    'test_globals.hpp',
    'test_items.cpp',
    'test_Input.cpp',
    'test_lifetime.cpp',
    'test_loaderdff.cpp',
    'test_LoaderIPL.cpp',
    'test_Logger.cpp',
    'test_menu.cpp',
    'test_object.cpp',
    'test_object_data.cpp',
    'test_pickup.cpp',
    'test_renderer.cpp',
    'test_rwbstream.cpp',
    'test_SaveGame.cpp',
    'test_scriptmachine.cpp',
    'test_state.cpp',
    'test_text.cpp',
    'test_trafficdirector.cpp',
    'test_vehicle.cpp',
    'test_VisualFX.cpp',
    'test_weapon.cpp',
    'test_world.cpp',
    'test_ZoneData.cpp',

    # Hack in rwgame sources until there's a per-target test suite
    join_paths(rwgame_path, 'GameConfig.cpp'),
    join_paths(rwgame_path, 'GameWindow.cpp'),
    join_paths(rwgame_path, 'GameInput.cpp'),
)
dep_boost_rwtests = dependency('boost', modules: ['filesystem', 'system', 'unit_test_framework'], required: true)

test_data_val = get_option('tests_nodata') ? 0 : 1
test_data_arg = '-DRW_TEST_WITH_DATA=@0@'.format(test_data_val)

rwtests = executable('rwtests', rwtests_sources, cpp_args: ['-DBOOST_TEST_DYN_LINK', test_data_arg], dependencies: [dep_bullet, dep_openal, dep_sdl2, dep_boost_rwtests], include_directories: [rwlib_include, rwengine_include, rwgame_include], link_with: [rwengine])
