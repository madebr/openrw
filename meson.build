project('OpenRW', ['c', 'cpp'], license: 'GPL3', meson_version: '>=0.44', default_options: ['buildtype=release', 'default_library=static', 'cpp_std=c++14'])

languages = ['c', 'cpp']

add_global_arguments('-Wall', '-pthread', '-Wextra', '-Wpedantic', language: languages)

if get_option('buildtype') == 'debug'
    add_global_arguments('-DRW_DEBUG=1', language: languages)
endif

if host_machine.system() == 'linux'
    add_global_arguments('-DRW_LINUX', language: languages)
elif host_machine.system() == 'windows'
    add_global_arguments('-DRW_WINDOWS', language: languages)
elif host_machine.system() == 'darwin'
    add_global_arguments('-DRW_OSX', language: languages)
elif host_machine.system() == 'bsd'
    add_global_arguments('-DRW_OPENBSD', language: languages)
    # FIXME: RW_FREEBSD, RW_OPENBSD, RW_NETBSD (needed?)
else
    error('Unknown host system')
endif

cpp_compiler = meson.get_compiler('cpp')

if cpp_compiler.get_id() == 'gcc'
    cpp_extra_args = []
    if host_machine.system() == 'windows'
        # mingw
        cpp_extra_args = ['-fpermissive']
    endif
elif cpp_compiler.get_id() == 'clang'
    cpp_extra_args = ['-Wno-gnu-array-member-paren-init']
else
    error('Unknown compiler')
endif

# Make GLM use radians
add_global_arguments('-DGLM_FORCE_RADIANS', language: languages)

if get_option('profiling')
    add_global_arguments('-DRENDER_PROFILER=0', '-DRW_PROFILER=1', language: languages)
else
    add_global_arguments('-DRENDER_PROFILER=0', '-DRW_PROFILER=0', language: languages)
endif

if get_option('script_debug')
    add_global_arguments('-DRW_SCRIPT_DEBUG', language: languages)
endif

if get_option('failed_check_action') == 'IGNORE'
    add_global_arguments('-DRW_FAILED_CHECK_ACTION=0', language: languages)
elif get_option('failed_check_action') == 'ABORT'
    add_global_arguments('-DRW_FAILED_CHECK_ACTION=1', language: languages)
elif get_option('failed_check_action') == 'BREAKPOINT'
    add_global_arguments('-DRW_FAILED_CHECK_ACTION=2', language: languages)
endif

if get_option('verbose_debug_messages')
    add_global_arguments('-DRW_VERBOSE_DEBUG_MESSAGES=1', language: languages)
else
    add_global_arguments('-DRW_VERBOSE_DEBUG_MESSAGES=0', language: languages)
endif

# find dependencies
dep_opengl = dependency('gl', required: true)
dep_openal = dependency('openal', required: true)
dep_bullet = dependency('bullet', required: true)
dep_glm = dependency('glm', required: true)
dep_avcodec = [
    dependency('libavcodec', required: true),
    dependency('libavformat', required: true),
    dependency('libavutil', required: true),
    dependency('libavresample', required: true),
]
dep_sdl2 = dependency('SDL2', required: true)

subdir('rwlib')
subdir('rwengine')
subdir('rwgame')

if get_option('build_viewer')
    subdir('rwviewer')
endif

if get_option('build_tests')
    subdir('tests')
endif

# Install License file
install_data('COPYING', install_dir: get_option('docdir'))
