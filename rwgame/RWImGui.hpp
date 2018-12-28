#ifndef RWGAME_RWIMGUI_HPP
#define RWGAME_RWIMGUI_HPP

#include <core/Logger.hpp>

#include "RWRingBuffer.hpp"

#include <SDL.h>

#include <tuple>

class RWGame;
struct ImGuiContext;

class RWImGui {
    RWGame &_game;
    ImGuiContext *_context = nullptr;
public:
    RWImGui(RWGame &game);
    ~RWImGui();
    void init();
    void destroy();
    std::tuple<bool, bool> process_event(SDL_Event &event);
    void tick();
};


class RWRingBufferLog final : public Logger::MessageReceiver {
static constexpr size_t N = 1024u;
RWRingBuffer<std::string, N> _log;
public:
    void messageReceived(const Logger::LogMessage& message) override;
    const RWRingBuffer<std::string, N>& getRingBuffer() const;
};

#endif // RWGAME_RWIMGUI_HPP
