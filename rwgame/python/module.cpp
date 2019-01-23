#include "pyopenrw_priv.hpp"

#include "pyopenrw.hpp"

#include <GitSHA1.h>

#include <pybind11/pybind11.h>
#include <pybind11/iostream.h>

namespace py = pybind11;

void register_streamutils(py::module& m) {
    py::module mutil = m.def_submodule("streamutils", "Bind utils");
    py::add_ostream_redirect(mutil, "ostream_redirect");
}

void pyopenrw_register_module(py::module& openrw) {
    py::module m = openrw.def_submodule("alpha", "Alpha submodule");

    m.attr("__version__")= kGitSHA1Hash;
    m.attr("EMBEDDED") = PYOPENRW_EMBEDDED;
    register_streamutils(m);

    register_logging(m);
    register_config(m);
    register_game(m);
}
