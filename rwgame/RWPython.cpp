#include "RWPython.hpp"

#include "pyopenrw.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

#include <iostream>

namespace py = pybind11;

class PyScopedStdStreamRedirect {
    py::object stdin_original;
    py::object stdout_original;
    py::object stderr_original;
    py::object stdin_buffer;
    py::object stdout_buffer;
    py::object stderr_buffer;
public:
    PyScopedStdStreamRedirect() {
        auto sysm = py::module::import("sys");
        stdin_original = sysm.attr("stdin");
        stdout_original = sysm.attr("stdout");
        stderr_original = sysm.attr("stderr");
        auto stringio = py::module::import("io").attr("StringIO");
        stdin_buffer = stringio();
        stdout_buffer = stringio();
        stderr_buffer = stringio();
        sysm.attr("stdin") = stdin_buffer;
        sysm.attr("stdout") = stdout_buffer;
        sysm.attr("stderr") = stderr_buffer;
    }
    size_t stdinWrite(const std::string& s) {
        return py::int_(stdin_buffer.attr("write")(s));
    }
    std::string stdoutString() {
        stdout_buffer.attr("seek")(0, 0);
        auto pyText = stdout_buffer.attr("read")();
        return py::str(pyText.attr("rstrip")("\n"));
    }
    std::string stderrString() {
        stderr_buffer.attr("seek")(0, 0);
        auto pyText = stderr_buffer.attr("read")();
        return py::str(pyText.attr("rstrip")("\n"));
    }
    ~PyScopedStdStreamRedirect() {
        auto sysm = py::module::import("sys");
        sysm.attr("stdin") = stdin_original;
        sysm.attr("stdout") = stdout_original;
        sysm.attr("stderr") = stderr_original;
    }
};

struct RWSubPythonInterpreter::SubInterpreter {
    py::object console;
public:
    SubInterpreter() {
        console = py::module::import("code").attr("InteractiveConsole")(py::dict(py::arg("filename")="RWConsole"));
    }
};

RWSubPythonInterpreter::RWSubPythonInterpreter()
    : m_sub_interpreter(std::make_unique<SubInterpreter>()) {
}

RWSubPythonInterpreter::~RWSubPythonInterpreter() = default;

RWSubPythonInterpreter::ExecutionResult RWSubPythonInterpreter::run(const std::string& s) {
    ExecutionResult result;
    PyScopedStdStreamRedirect scopedStdRedirect;
    try {
        m_sub_interpreter->console.attr("push")(s);
    } catch (std::runtime_error& e) {
        result.py_except = e.what();
    }

    auto buffer = py::list(m_sub_interpreter->console.attr("buffer"));
    m_sub_interpreter->console.attr("resetbuffer")();
    std::cerr << "buffer: " << static_cast<std::string>(py::repr(buffer)) << "\n";

    std::string s_new;
    s_new.reserve(s.size());
    for (size_t i = 0u; i < buffer.size(); ++i) {
        s_new.append(py::str(buffer[i]));
        if (i != buffer.size() - 1) {
            s_new.append("\n");
        }
    }

    auto s_end_pos = s.rfind(s_new);
    auto s_executed = s.substr(0, s_end_pos);
    s_executed.erase(s_executed.find_last_not_of(" \t\r\n") + 1);

    result.py_executed = std::move(s_executed);
    result.py_remaining = std::move(s_new);
    result.py_stdout = scopedStdRedirect.stdoutString();
    result.py_stderr = scopedStdRedirect.stderrString();

    return result;
}
