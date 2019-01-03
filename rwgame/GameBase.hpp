#ifndef RWGAME_GAMEBASE_HPP
#define RWGAME_GAMEBASE_HPP
#include "GameWindow.hpp"
#include "RWConfig.hpp"

#include <core/Logger.hpp>

#include <memory>

#ifdef RW_PYTHON
namespace pybind11 {
class scoped_interpreter;
}
#endif

/**
 * @brief Handles basic window and setup
 */
class GameBase {
public:
    GameBase(Logger& inlog, const std::optional<RWArgConfigLayer> &args);

    virtual ~GameBase() = 0;

    GameWindow& getWindow() {
        return window;
    }

    const RWConfig& getConfig() const {
        return config;
    }

protected:
    RWConfig buildConfig(const std::optional<RWArgConfigLayer> &args);
    Logger& log;
    GameWindow window{};
    RWConfig config{};
#ifdef RW_PYTHON
    std::unique_ptr<pybind11::scoped_interpreter> python_guard;
#endif
};

#endif
