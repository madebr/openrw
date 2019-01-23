#include "pyopenrw.hpp"
#include "pyopenrw_priv.hpp"

#include "RWPython.hpp"

#include <pybind11/embed.h>

const bool PYOPENRW_EMBEDDED = true;

class RWScopedPythonInterpreter::ScopedInterpreter {
    py::scoped_interpreter m_scoped_interpreter;
};

void RWScopedPythonInterpreter::create() {
    static std::weak_ptr<ScopedInterpreter> weakScopedInterpreter;
    m_scoped_interpreter = weakScopedInterpreter.lock();
    if (!m_scoped_interpreter) {
        m_scoped_interpreter = std::make_shared<ScopedInterpreter>();
        weakScopedInterpreter = m_scoped_interpreter;
    }
}

PYBIND11_EMBEDDED_MODULE(PYOPENRW_NAME, m) {
    pyopenrw_register_module(m);
}
