#ifndef GAMEWINDOW_HPP
#define GAMEWINDOW_HPP

#include <glm/vec2.hpp>
#include <string>
#include <SDL.h>

#include <render/GameRenderer.hpp>

class GameWindow {
    SDL_Window* window = nullptr;
    SDL_Surface* icon = nullptr;
    SDL_GLContext glcontext{nullptr};
public:
    GameWindow() = default;

    void create(const std::string& title, size_t w, size_t h, bool fullscreen);
    void close();

    void showCursor();
    void hideCursor();

    glm::ivec2 getSize() const;

    void show() {
        SDL_ShowWindow(window);
    }

    void hide() {
        SDL_HideWindow(window);
    }

    void swap() const {
        SDL_GL_SwapWindow(window);
    }

    bool isOpen() const {
        return !!window;
    }
};

#endif
