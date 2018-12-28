#include "RWImGui.hpp"

#include <core/Logger.hpp>

#include "RWGame.hpp"
#include "RWRingBuffer.hpp"

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#include <gl/gl_core_3_3.h>

#include <sstream>

void RWRingBufferLog::messageReceived(const Logger::LogMessage& message) {
    std::ostringstream oss;
    oss << Logger::severityChar[message.severity] << " [" << message.component << "] " << message.message;
    _log.add(oss.str());
}

const RWRingBuffer<std::string, RWRingBufferLog::N>& RWRingBufferLog::getRingBuffer() const {
    return _log;
}


RWImGui::RWImGui(RWGame &game)
    : _game(game) {
}

RWImGui::~RWImGui() {
    destroy();
}

void RWImGui::init() {
    IMGUI_CHECKVERSION();
    _context = ImGui::CreateContext();

    auto [window, context] = _game.getWindow().getSDLContext();

    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 150");
}

void RWImGui::destroy() {
    if (!_context) {
        return;
    }
    ImGui::SetCurrentContext(_context);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    _context = nullptr;
}

std::tuple<bool, bool> RWImGui::process_event(SDL_Event &event) {
    if (!_context) {
        return std::make_tuple(false, false);
    }
    ImGui::SetCurrentContext(_context);
    ImGui_ImplSDL2_ProcessEvent(&event);
    auto& io = ImGui::GetIO();
    return std::make_tuple(io.WantCaptureMouse, io.WantCaptureKeyboard);
}

void log_windowdraw(RWGame& rwgame, bool* open) { // const char* title, bool* p_open = NULL) {
    ImGui::SetNextWindowSize(ImVec2(500,400), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("OpenRW Log", open)) {
        ImGui::End();
        return;
    }
    ImGui::BeginChild("Log");
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
    const auto& ring = rwgame.getRingBufferLog().getRingBuffer();
    ImGuiListClipper clipper(ring.size());
    while (clipper.Step()) {
        for (auto& s : ring.slice(clipper.DisplayStart, clipper.DisplayEnd)) {
            ImGui::Text(s.c_str());
        }
    }
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::End();
}


void RWImGui::tick() {
    if (!_context) {
        return;
    }
    ImGui::SetCurrentContext(_context);
    auto& io = ImGui::GetIO();

    auto [window, sdl_glcontext] = _game.getWindow().getSDLContext();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    static float f = 0.0f;

    ImGui::Begin("Hello, world!");
    ImGui::Text("Hello, world!");
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", static_cast<double>(1000.0f / io.Framerate), static_cast<double>(io.Framerate));   ImGui::End();

    static bool logopen = false;
    log_windowdraw(_game, &logopen);
    static bool show_demo_window = true;
    ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
