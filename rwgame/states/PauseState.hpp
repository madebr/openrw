#ifndef PAUSESTATE_HPP
#define PAUSESTATE_HPP

#include "StateManager.hpp"

class PauseState final : public State {
public:
    PauseState(RWGame* game);

    void enter() override;

    void exit() override;

    void tick(float dt) override;

    void draw(GameRenderer* r) override;

    void handleEvent(const SDL_Event& e) override;
};

#endif  // PAUSESTATE_HPP
