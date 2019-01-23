#ifndef RWGAME_RWIMGUI_HPP
#define RWGAME_RWIMGUI_HPP

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

    void show(bool b=true);
    bool visible() const;

    std::tuple<bool, bool> process_event(SDL_Event &event);
    void tick();
};


#endif // RWGAME_RWIMGUI_HPP
