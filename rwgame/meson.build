rwgame_sources = files(
    'GitSHA1.h',

    'main.cpp',

    'GameBase.hpp',
    'GameBase.cpp',

    'RWGame.cpp',

    'GameConfig.cpp',
    'GameWindow.cpp',

    'StateManager.hpp',
    'StateManager.cpp',
    'State.cpp',

    'MenuSystem.hpp',
    'MenuSystem.cpp',

    'GameInput.hpp',
    'GameInput.cpp',

    'states/LoadingState.hpp',
    'states/LoadingState.cpp',
    'states/IngameState.hpp',
    'states/IngameState.cpp',
    'states/PauseState.hpp',
    'states/PauseState.cpp',
    'states/MenuState.hpp',
    'states/MenuState.cpp',
    'states/DebugState.hpp',
    'states/DebugState.cpp',
    'states/BenchmarkState.hpp',
    'states/BenchmarkState.cpp',

    'DrawUI.cpp',
)

git_sha1 = vcs_tag(command: ['git', 'rev-parse', 'HEAD'], input: 'GitSHA1.cpp.in', output: 'GitSHA1.cpp', replace_string: '@GIT_SHA1@')

dep_boost_rwgame = dependency('boost', modules: ['program_options'])

rwgame_include = include_directories('.')
rwgame_path = meson.current_source_dir()

rwgame = executable('rwgame', rwgame_sources, git_sha1, dependencies: [dep_boost_rwgame, dep_bullet, dep_openal, dep_sdl2], include_directories: [rwengine_include, rwlib_include], link_with: [rwengine])
