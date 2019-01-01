#ifndef RWGAME_GAMEBASE_HPP
#define RWGAME_GAMEBASE_HPP
#include "GameWindow.hpp"
#include "RWConfig.hpp"
#include "RWImGui.hpp"

#include <core/Logger.hpp>

#include <map>

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

    RWRingBufferLog &getRingBufferLog() {
        return ringbufferlog;
    }

protected:
    RWConfig buildConfig(const std::optional<RWArgConfigLayer> &args);
    Logger& log;
#ifdef RW_IMGUI
    RWRingBufferLog ringbufferlog;
#endif
    GameWindow window{};
    RWConfig config{};
};

#endif
