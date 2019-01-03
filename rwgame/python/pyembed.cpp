#include "pyopenrw.hpp"
#include "pyopenrw_priv.hpp"

#include <pybind11/embed.h>

const bool PYOPENRW_EMBEDDED = true;

PYBIND11_EMBEDDED_MODULE(PYOPENRW_NAME, m) {
    pyopenrw_register_module(m);
}
