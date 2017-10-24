rwlib_sources = files(
    # GL stuff is only here temporarily, hoping to move it back to rwengine
    'source/gl/gl_core_3_3.c',
    'source/gl/gl_core_3_3.h',
    'source/gl/DrawBuffer.hpp',
    'source/gl/DrawBuffer.cpp',
    'source/gl/GeometryBuffer.hpp',
    'source/gl/GeometryBuffer.cpp',
    'source/gl/TextureData.hpp',
    'source/gl/TextureData.cpp',

    'source/rw/abort.cpp',
    'source/rw/forward.hpp',
    'source/rw/types.hpp',
    'source/rw/defines.hpp',

    'source/platform/FileHandle.hpp',
    'source/platform/FileIndex.hpp',
    'source/platform/FileIndex.cpp',

    'source/data/Clump.hpp',
    'source/data/Clump.cpp',

    'source/loaders/LoaderIMG.hpp',
    'source/loaders/LoaderIMG.cpp',
    'source/loaders/RWBinaryStream.hpp',
    'source/loaders/LoaderDFF.hpp',
    'source/loaders/LoaderDFF.cpp',
    'source/loaders/LoaderSDT.hpp',
    'source/loaders/LoaderSDT.cpp',
    'source/loaders/LoaderTXD.hpp',
    'source/loaders/LoaderTXD.cpp',
)

dep_boost_rwlib = dependency('boost', modules: ['filesystem', 'system'])

rwlib_include = include_directories('source')

rwlib = library('rwlib', rwlib_sources, dependencies: [dep_opengl, dep_boost_rwlib], include_directories: [rwlib_include])
