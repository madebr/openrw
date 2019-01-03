#include "pyopenrw.hpp"
#include "pyopenrw_priv.hpp"

#include <core/Logger.hpp>
#include <RWGame.hpp>

#include <SDL.h>

const bool PYOPENRW_EMBEDDED = false;

void start_openrw() {
    Logger logger;
    RWGame rwgame(logger, std::nullopt);
    rwgame.run();
}

PYBIND11_MODULE(PYOPENRW_NAME, m) {
    SDL_SetMainReady();
    pyopenrw_register_module(m);

    m.def("start_openrw", start_openrw);
}
