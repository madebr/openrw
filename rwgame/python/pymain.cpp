#include "pyopenrw.hpp"
#include "pyopenrw_priv.hpp"

#include <core/Logger.hpp>
#include <RWGame.hpp>

#include <SDL.h>

const bool PYOPENRW_EMBEDDED = false;

class PyRWGame {
public:
    PyRWGame(std::shared_ptr<Logger> logger)
    : _logger(logger)
    , _game(*logger, std::nullopt) {
    }
    PyRWGame()
    : PyRWGame(std::make_shared<Logger>()) {
    }
    void run() {
        _game.run();
    }
private:
    std::shared_ptr<Logger> _logger;
    RWGame _game;
};

PYBIND11_MODULE(PYOPENRW_NAME, m) {
    SDL_SetMainReady();
    pyopenrw_register_module(m);

    py::class_<PyRWGame>(m, "RWGame")
            .def(py::init<>())
            .def(py::init<std::shared_ptr<Logger>>(), py::arg("logger"))
            .def("run", &PyRWGame::run);
}
