#include "RWImGui.hpp"

#include <core/Logger.hpp>

#include "RWGame.hpp"
#include "RWRingBufferLog.hpp"

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <misc/cpp/imgui_stdlib.h>

#include <gl/gl_core_3_3.h>

#ifdef RW_PYTHON
#include <pybind11/embed.h>
namespace py = pybind11;
#endif

#include <array>
#include <iostream>

struct RWImGui::RWImGuiState {
    bool show_console = false;
    bool show_demo_window = false;
    std::string log_input_buffer;
    bool log_wrap = true;
#ifdef RW_PYTHON
    py::object log_console;
#endif
};

RWImGui::RWImGui(RWGame &game)
    : _game(game) {
    _state = std::make_unique<RWImGui::RWImGuiState>();
#ifdef RW_PYTHON
    _state->log_console = py::module::import("code").attr("InteractiveConsole")(py::dict(py::arg("filename")="RWConsole"));
#endif
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

void RWImGui::show(bool b) {
    _state->show_console = b;
}

bool RWImGui::visible() const {
    return _state->show_console;
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

namespace {

const std::array<ImVec4, static_cast<size_t>(RWRingBufferLog::Message::MessageLevel::_Count)> messageColors = {
    {
        ImVec4(0.2f, 0.2f, 0.2f, 1.0f),
        ImVec4(0.4f, 0.4f, 0.4f, 1.0f),
        ImVec4(0.96f, 0.62f, 0.26f, 1.0f),
        ImVec4(1.0f, 0.4f, 0.4f, 1.0f),
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
        ImVec4(1.0f, 0.4f, 0.4f, 1.0f),
    }};

class PyScopedStdStreamRedirect {
    py::object _stdin;
    py::object _stdout;
    py::object _stderr;
    py::object _stdin_buffer;
    py::object _stdout_buffer;
    py::object _stderr_buffer;
public:
    PyScopedStdStreamRedirect() {
        auto sysm = py::module::import("sys");
        _stdin = sysm.attr("stdin");
        _stdout = sysm.attr("stdout");
        _stderr = sysm.attr("stderr");
        auto stringio = py::module::import("io").attr("StringIO");
        _stdin_buffer = stringio();
        _stdout_buffer = stringio();
        _stderr_buffer = stringio();
        sysm.attr("stdin") = _stdin_buffer;
        sysm.attr("stdout") = _stdout_buffer;
        sysm.attr("stderr") = _stderr_buffer;
    }
    size_t stdinWrite(const std::string& s) {
        return py::int_(_stdin_buffer.attr("write")(s));
    }
    std::string stdoutString() {
        _stdout_buffer.attr("seek")(0, 0);
        auto pyText = _stdout_buffer.attr("read")();
        return py::str(pyText.attr("rstrip")("\n"));
    }
    std::string stderrString() {
        _stderr_buffer.attr("seek")(0, 0);
        auto pyText = _stderr_buffer.attr("read")();
        return py::str(pyText.attr("rstrip")("\n"));
    }
    ~PyScopedStdStreamRedirect() {
        auto sysm = py::module::import("sys");
        sysm.attr("stdout") = _stdout;
        sysm.attr("stderr") = _stderr;
    }
};

namespace {
    static constexpr unsigned MULTILINE_NB = 5;
}
void log_consoledraw(RWGame& rwgame, bool* open, RWImGui::RWImGuiState &state) { // const char* title, bool* p_open = NULL) {
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
    if (!ImGui::Begin("OpenRW Console", open, window_flags)) {
        ImGui::End();
        return;
    }
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Clear log")) {
                rwgame.getRingBufferLog().getRingBuffer().clear();
            }
            ImGui::Separator();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Wrap lines", "", &state.log_wrap);
            ImGui::Separator();
            ImGui::MenuItem("ImGui Demo Window", "", &state.show_demo_window);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::Spacing();

    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + MULTILINE_NB * ImGui::GetTextLineHeight();
    ImGui::BeginChild("ConsoleScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
    const auto& ring = rwgame.getRingBufferLog().getRingBuffer();
    ImGuiListClipper clipper(ring.size());
    while (clipper.Step()) {
        for (auto& msg : ring.slice(clipper.DisplayStart, clipper.DisplayEnd)) {
            const auto& msgColor = messageColors[static_cast<size_t>(msg.level)];
            ImGui::PushStyleColor(ImGuiCol_Text, msgColor);
            if (state.log_wrap) {
                ImGui::TextWrapped(msg.text.c_str());
            } else {
                ImGui::Text(msg.text.c_str());
            }
            ImGui::PopStyleColor();
        }
    }
    if (rwgame.getRingBufferLog().updated) {
        ImGui::SetScrollHereY(1.0f);
        rwgame.getRingBufferLog().updated = false;
    }
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();
    bool reclaim_focus = false;

    ImGui::Text(">>>");
    ImGui::SameLine();

    if (ImGui::InputTextMultiline("##command_multiline", &state.log_input_buffer, ImVec2(-1.f, MULTILINE_NB * ImGui::GetTextLineHeight()), ImGuiInputTextFlags_EnterReturnsTrue, [](auto) {return 0;}, nullptr)) {
        auto& s = state.log_input_buffer;
//        s.erase(s.find_last_not_of(" \t\r\n") + 1);

#ifdef RW_PYTHON
        {
            PyScopedStdStreamRedirect pyOutputRedirect{};
            try {
                auto push_result = state.log_console.attr("push")(s);
            } catch (std::runtime_error& e) {
                rwgame.getLogger().info("RWImGui", e.what());
                rwgame.getLogger().info("RWImGui", "SystemExit event received -> exiting");
                rwgame.getStateManager().clear();
            }

            auto buffer = py::list(state.log_console.attr("buffer"));
            state.log_console.attr("resetbuffer")();
            std::string s_new;
            s_new.reserve(s.size());
            for (size_t i = 0u; i < buffer.size(); ++i) {
                s_new.append(py::str(buffer[i]));
                if (i != buffer.size() - 1) {
                    s_new.append("\n");
                }
            }

            auto s_end_pos = s.rfind(s_new);
            auto s_executed = s.substr(0, s_end_pos);
            s_executed.erase(s_executed.find_last_not_of(" \t\r\n") + 1);

            std::cerr << "s = " << s << std::endl;
            std::cerr << "s_new = " << s_new << std::endl;
            std::cerr << "s_executed = " << s_executed << std::endl;

            if (s_executed.size() > 0) {
                rwgame.getRingBufferLog().input(">>> " + s_executed);
                s = std::move(s_new);//.erase(0, s_end_pos);
            }

            auto sout = pyOutputRedirect.stdoutString();
            if (sout.size()) {
                rwgame.getRingBufferLog().toStdOut(sout);
            }
            auto serr = pyOutputRedirect.stderrString();
            if (serr.size()) {
                rwgame.getRingBufferLog().toStdErr(serr);
            }
        }
#else
        rwgame.getRingBufferLog().input(">>> " + s);
        rwgame.getRingBufferLog().messageReceived({"console", Logger::MessageSeverity::Error, "console commands not supported"});
#endif
        reclaim_focus = true;
        rwgame.getRingBufferLog().updated = true;
    }
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus) {
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
    }

    ImGui::End();
}
}

void RWImGui::tick() {
    if (!_context) {
        return;
    }
    ImGui::SetCurrentContext(_context);

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    auto [window, sdl_glcontext] = _game.getWindow().getSDLContext();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    if (_state->show_console) {
        log_consoledraw(_game, &_state->show_console, *_state);
    }
    if (_state->show_demo_window) {
        ImGui::ShowDemoWindow(&_state->show_demo_window);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
