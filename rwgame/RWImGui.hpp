#ifndef RWGAME_RWIMGUI_HPP
#define RWGAME_RWIMGUI_HPP

#include <core/Logger.hpp>

#include "RWRingBuffer.hpp"

#include <SDL.h>

#include <memory>
#include <tuple>

class RWGame;
struct ImGuiContext;

class RWImGui {
public:
    struct RWImGuiState;
private:
    RWGame &_game;
    ImGuiContext *_context = nullptr;
    std::unique_ptr<RWImGuiState> _state;
public:
    RWImGui(RWGame &game);
    ~RWImGui();
    void init();
    void destroy();
    std::tuple<bool, bool> process_event(SDL_Event &event);
    void tick();
};


class RWRingBufferLog final : public Logger::MessageReceiver, std::enable_shared_from_this<RWRingBufferLog> {
public:
struct Message {
    enum MessageLevel {
        VERBOSE = 0,
        INFO,
        WARNING,
        ERROR,
        INPUT,
        STDOUT,
        STDERR,
        _Count,
    };
    std::string text;
    MessageLevel level;
};
private:
static constexpr size_t N = 1024u;
RWRingBuffer<Message, N> _log;
public:
    bool updated = false;
    void messageReceived(const Logger::LogMessage& message) override;
    void input(const std::string txt);
    void toStdOut(const std::string txt);
    void toStdErr(const std::string txt);
    const RWRingBuffer<Message, N>& getRingBuffer() const;
    RWRingBuffer<Message, N>& getRingBuffer();
};

#endif // RWGAME_RWIMGUI_HPP
