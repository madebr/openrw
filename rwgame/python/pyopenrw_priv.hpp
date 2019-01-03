#ifndef RWGAME_PYTHON_PYOPENRW_IMPL_HPP
#define RWGAME_PYTHON_PYOPENRW_IMPL_HPP

#include <pybind11/pybind11.h>
namespace py = pybind11;

void register_logging(py::module& parent);
void pyopenrw_register_module(py::module& module);

#endif // RWGAME_PYTHON_PYOPENRW_IMPL_HPP
