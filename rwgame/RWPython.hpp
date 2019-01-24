#ifndef RWGAME_RWPYTHON_HPP
#define RWGAME_RWPYTHON_HPP

#include <memory>

class RWScopedPythonInterpreter {
    class ScopedInterpreter;
    std::shared_ptr<ScopedInterpreter> m_scoped_interpreter;
public:
    void create();
};


class RWSubPythonInterpreter {
    class SubInterpreter;
    std::unique_ptr<SubInterpreter> m_sub_interpreter;
public:
    RWSubPythonInterpreter();
    RWSubPythonInterpreter(RWSubPythonInterpreter &) = delete;
    ~RWSubPythonInterpreter();

    struct ExecutionResult {
        std::string py_executed;
        std::string py_remaining;
        std::string py_stdout;
        std::string py_stderr;
        std::string py_except;
    };
    ExecutionResult run(const std::string& s);
};

#endif // RWGAME_RWPYTHON_HPP
