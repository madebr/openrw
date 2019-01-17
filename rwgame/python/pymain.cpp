#include "pyopenrw.hpp"
#include "pyopenrw_priv.hpp"

#include <core/Logger.hpp>

#include <SDL.h>

const bool PYOPENRW_EMBEDDED = false;

PYBIND11_MODULE(PYOPENRW_NAME, m) {
    SDL_SetMainReady();
    pyopenrw_register_module(m);
}
