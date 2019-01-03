#include "pyopenrw_priv.hpp"

#include <GitSHA1.h>

#include <pybind11/pybind11.h>
#include <pybind11/iostream.h>

namespace py = pybind11;

void pyopenrw_register_module(py::module& m) {
    m.attr("__version__")= kGitSHA1Hash;

    py::module mutil = m.def_submodule("bindutils", "Bind utils");
    py::add_ostream_redirect(mutil, "ostream_redirect");

    register_logging(m);
    register_config(m);
}
