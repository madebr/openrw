#ifndef RWGAME_GAMEBASE_HPP
#define RWGAME_GAMEBASE_HPP
#include "GameWindow.hpp"
#include "RWConfig.hpp"
#include "RWPython.hpp"

#include <core/Logger.hpp>

#include <memory>

#ifdef RW_IMGUI
class RWRingBufferLog;
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

    Logger& getLogger() {
        return log;
    }

#ifdef RW_IMGUI
    RWRingBufferLog &getRingBufferLog() {
        return *ringbufferlog;
    }
#endif

protected:
    RWConfig buildConfig(const std::optional<RWArgConfigLayer> &args);
    Logger& log;
#ifdef RW_IMGUI
    std::shared_ptr<RWRingBufferLog> ringbufferlog;
#endif
    GameWindow window{};
    RWConfig config{};
#ifdef RW_PYTHON
    RWScopedPythonInterpreter scopedPythonIterpreter;
#endif
};

#endif
