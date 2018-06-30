#include "GameWindow.hpp"
#include <core/Logger.hpp>

#include <rw/defines.hpp>

#include <stb_image.h>
#include <logo64.h>

GameWindow::GameWindow() : window(nullptr), glcontext(nullptr) {
}

void GameWindow::create(const std::string& title, size_t w, size_t h,
                        bool fullscreen) {
    Uint32 style = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    if (fullscreen) style |= SDL_WINDOW_FULLSCREEN;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, w, h, style);
    if (window == nullptr) {
        // Window creation failure is fatal
        std::string sdlErrorStr = SDL_GetError();
        throw std::runtime_error("SDL_CreateWindow failed: " + sdlErrorStr);
    }
    glcontext = SDL_GL_CreateContext(window);
    if (glcontext == nullptr) {
        // context creation failure is fatal
        std::string sdlErrorStr = SDL_GetError();
        throw std::runtime_error("SDL_GL_CreateContext failed: " + sdlErrorStr);
    }
    int windowIconWidth, windowIconHeight, channels;
    auto windowIconData = stbi_load_from_memory(LOGO64, sizeof(LOGO64), &windowIconWidth, &windowIconHeight, &channels, 4);
    if (channels != 4) {
        RW_ERROR("Icon expected 4 channels. Got " << channels << ".");
    }
    Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    // Big Endian
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    // Little Endian
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
    icon = SDL_CreateRGBSurfaceFrom((void*)windowIconData,
                                    windowIconWidth, windowIconHeight,
                                    32, windowIconWidth * (32 / 8),
                                    rmask, gmask, bmask, amask);
    SDL_SetWindowIcon(window, icon);

    stbi_image_free(windowIconData);

    SDL_ShowWindow(window);
}

void GameWindow::close() {
    SDL_GL_DeleteContext(glcontext);
    SDL_FreeSurface(icon);
    SDL_DestroyWindow(window);

    window = nullptr;
}

void GameWindow::showCursor() {
    SDL_SetRelativeMouseMode(SDL_FALSE);
}

void GameWindow::hideCursor() {
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

glm::ivec2 GameWindow::getSize() const {
    int x, y;
    SDL_GL_GetDrawableSize(window, &x, &y);

    return glm::ivec2(x, y);
}
