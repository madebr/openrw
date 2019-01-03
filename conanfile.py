from conans import ConanFile, CMake


class OpenrwConan(ConanFile):
    name = 'openrw'
    version = 'master'
    license = 'GPL3'
    url = 'https://github.com/rwengine/openrw'
    description = "OpenRW 'Open ReWrite' is an un-official open source recreation of the classic Grand Theft Auto III game executable"
    settings = 'os', 'compiler', 'build_type', 'arch'
    options = {
        'shared': [True, False],
        'fPIC': [True, False],
        'test_data': [True, False],
        'viewer': [True, False],
        'tools': [True, False],
        'python': [True, False],
        'profiling': [True, False],
    }

    default_options = {
        'shared': False,
        'fPIC': True,
        'test_data': False,
        'viewer': True,
        'tools': True,
        'python': True,
        'profiling': True,
        'bullet3:shared': False,
        'sdl2:sdl2main': False,
    }

    generators = 'cmake',
    exports_sources = 'CMakeLists.txt', 'cmake_configure.cmake', 'cmake_options.cmake', 'CMakeCPack.cmake', 'COPYING', \
                      'cmake/modules/*', 'benchmarks', 'rwcore/*', 'rwengine/*', 'rwgame/*', 'rwviewer/*', \
                      'rwtools/*', 'tests/*', 'external/*'

    _rw_dependencies = {
        'game': (
            'openal/1.19.0@bincrafters/stable',
            'bullet3/2.87@bincrafters/stable',
            'glm/0.9.9.1@g-truc/stable',
            'ffmpeg/4.0.2@bincrafters/stable',
            'sdl2/2.0.9@bincrafters/stable',
            'boost/1.68.0@conan/stable',
        ),
        'viewer': (
            'qt/5.12.0@bincrafters/stable',
        ),
        'tools': (
            'freetype/2.9.0@bincrafters/stable',
        ),
        'python': (
            'pybind11/2.2.3@conan/stable',
        ),
    }

    def config_options(self):
        if self.settings.compiler == 'Visual Studio':
            del self.options.fPIC

    def configure(self):
        if self.options.viewer:
            self.options['qt'].opengl = 'desktop'

    def requirements(self):
        for dep in self._rw_dependencies['game']:
            self.requires(dep)
        if self.options.viewer:
            for dep in self._rw_dependencies['viewer']:
                self.requires(dep)
        if self.options.tools:
            for dep in self._rw_dependencies['tools']:
                self.requires(dep)
        if self.options.python:
            for dep in self._rw_dependencies['python']:
                self.requires(dep)

    def _configure_cmake(self):
        cmake = CMake(self)
        fpic = self.options.python
        if self.settings.compiler == 'Visual Studio':
            fpic |= self.options.fPIC
        defs = {
            'BUILD_SHARED_LIBS': self.options.shared,
            'CMAKE_BUILD_TYPE': self.settings.build_type,
            'BUILD_TESTS': True,
            'BUILD_VIEWER': self.options.viewer,
            'BUILD_TOOLS': self.options.tools,
            'BUILD_PYTHON': self.options.python,
            'WITH_PIC': fpic,
            'TESTS_NODATA': not self.options.test_data,
            'ENABLE_PROFILING': self.options.profiling,
            'USE_CONAN': True,
            'BOOST_STATIC': not self.options['boost'].shared,
        }

        cmake.configure(defs=defs)
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        if self.options.viewer:
            # FIXME: https://github.com/osechet/conan-qt/issues/6 and https://github.com/conan-io/conan/issues/2619
            self.copy('qt.conf', dst='bin', src='rwviewer')
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ['rwgame', 'rwengine', 'rwcore']
        self.cpp_info.stdcpp = 17
