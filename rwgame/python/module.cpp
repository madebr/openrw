#include "pyopenrw_priv.hpp"

#include <GitSHA1.h>

#include <pybind11/pybind11.h>

namespace py = pybind11;

void pyopenrw_register_module(py::module& m) {
    m.attr("__version__")= kGitSHA1Hash;

    register_logging(m);
}
