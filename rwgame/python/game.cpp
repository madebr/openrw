#include <RWGame.hpp>

#include "pyopenrw.hpp"

#include <pybind11/pybind11.h>

namespace py = pybind11;

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

void register_game(py::module& m) {
    py::class_<PyRWGame> pyrwgame(m, "RWGame");
    pyrwgame
            .def_property_readonly("logger", [](RWGame& game) {return game.getLogger();})
            .def_property_readonly("config", [](RWGame& game) {return game.getConfig();});
    if (!PYOPENRW_EMBEDDED) {
        pyrwgame
                .def(py::init<>())
                .def(py::init<std::shared_ptr<Logger>>(), py::arg("logger"))
                .def("run", &PyRWGame::run);
    }
}
