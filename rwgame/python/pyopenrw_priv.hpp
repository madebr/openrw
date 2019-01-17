#ifndef RWGAME_PYTHON_PYOPENRW_IMPL_HPP
#define RWGAME_PYTHON_PYOPENRW_IMPL_HPP

#include <pybind11/pybind11.h>
namespace py = pybind11;

void pyopenrw_register_module(py::module& m);

void register_logging(py::module& m);
void register_config(py::module& m);
void register_game(py::module& m);

#endif // RWGAME_PYTHON_PYOPENRW_IMPL_HPP
