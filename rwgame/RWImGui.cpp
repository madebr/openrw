#include "RWImGui.hpp"

#include <core/Logger.hpp>

#include "RWGame.hpp"
#include "RWRingBuffer.hpp"

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#include <gl/gl_core_3_3.h>

#ifdef RW_PYTHON
#include <pybind11/embed.h>
namespace py = pybind11;
#endif

#include <array>
#include <iostream>
#include <sstream>

void RWRingBufferLog::messageReceived(const Logger::LogMessage& message) {
    std::ostringstream oss;
    oss << Logger::severityChar[message.severity] << " [" << message.component << "] " << message.message;
    _log.add({oss.str(), RWRingBufferLog::Message::MessageLevel(message.severity)});
    updated = true;
}

void RWRingBufferLog::input(const std::string txt) {
    _log.add({std::move(txt), RWRingBufferLog::Message::MessageLevel::INPUT});
    updated = true;
}

void RWRingBufferLog::toStdOut(const std::string txt) {
    _log.add({std::move(txt), RWRingBufferLog::Message::MessageLevel::STDOUT});
    updated = true;
}

void RWRingBufferLog::toStdErr(const std::string txt) {
    _log.add({std::move(txt), RWRingBufferLog::Message::MessageLevel::STDERR});
    updated = true;
}

const RWRingBuffer<RWRingBufferLog::Message, RWRingBufferLog::N>& RWRingBufferLog::getRingBuffer() const {
    return _log;
}

static_assert(static_cast<int>(Logger::MessageSeverity::Verbose) == static_cast<int>(RWRingBufferLog::Message::MessageLevel::VERBOSE));
static_assert(static_cast<int>(Logger::MessageSeverity::Info)    == static_cast<int>(RWRingBufferLog::Message::MessageLevel::INFO));
static_assert(static_cast<int>(Logger::MessageSeverity::Warning) == static_cast<int>(RWRingBufferLog::Message::MessageLevel::WARNING));
static_assert(static_cast<int>(Logger::MessageSeverity::Error)   == static_cast<int>(RWRingBufferLog::Message::MessageLevel::ERROR));


struct RWImGui::RWImGuiState {
    bool show_demo_window = false;
    bool show_log = false;
    std::array<char, 4096> log_input_buffer;
};

RWImGui::RWImGui(RWGame &game)
    : _game(game)
    , _state(std::make_unique<RWImGui::RWImGuiState>()) {
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

class PyStdErrOutStreamRedirect {
    py::object _stdout;
    py::object _stderr;
    py::object _stdout_buffer;
    py::object _stderr_buffer;
public:
    PyStdErrOutStreamRedirect() {
        auto sysm = py::module::import("sys");
        _stdout = sysm.attr("stdout");
        _stderr = sysm.attr("stderr");
        auto stringio = py::module::import("io").attr("StringIO");
        _stdout_buffer = stringio();
        _stderr_buffer = stringio();
        sysm.attr("stdout") = _stdout_buffer;
        sysm.attr("stderr") = _stderr_buffer;
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
    ~PyStdErrOutStreamRedirect() {
        auto sysm = py::module::import("sys");
        sysm.attr("stdout") = _stdout;
        sysm.attr("stderr") = _stderr;
    }
};

void log_windowdraw(RWGame& rwgame, bool* open, RWImGui::RWImGuiState &state) { // const char* title, bool* p_open = NULL) {
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("OpenRW Log", open)) {
        ImGui::End();
        return;
    }

    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
    ImGui::BeginChild("ConsoleScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
    const auto& ring = rwgame.getRingBufferLog().getRingBuffer();
    ImGuiListClipper clipper(ring.size());
    while (clipper.Step()) {
        for (auto& msg : ring.slice(clipper.DisplayStart, clipper.DisplayEnd)) {
            const auto& msgColor = messageColors[static_cast<size_t>(msg.level)];
            ImGui::PushStyleColor(ImGuiCol_Text, msgColor);
            ImGui::Text(msg.text.c_str());
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
    if (ImGui::InputText("command", state.log_input_buffer.data(), state.log_input_buffer.size(), ImGuiInputTextFlags_EnterReturnsTrue|ImGuiInputTextFlags_CallbackCompletion|ImGuiInputTextFlags_CallbackHistory, [](auto) {return 0;}, nullptr))
    {
        auto& s = state.log_input_buffer;
        rwgame.getRingBufferLog().input(std::string("> ") + s.data());
#ifdef RW_PYTHON
        {
            PyStdErrOutStreamRedirect pyOutputRedirect{};
            try {

                auto res = py::eval(s.data());
                if (!res.is_none()) {
                    rwgame.getRingBufferLog().toStdOut(py::repr(res));
                }
            } catch (const std::runtime_error& e) {
                rwgame.getRingBufferLog().messageReceived({"console", Logger::MessageSeverity::Error, "An exception occured"});
                rwgame.getRingBufferLog().messageReceived({"console", Logger::MessageSeverity::Error, e.what()});
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
        rwgame.getRingBufferLog().messageReceived({"console", Logger::MessageSeverity::Error, "console commands not supported"});
#endif
        strcpy(s.data(), "");
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

    auto [window, sdl_glcontext] = _game.getWindow().getSDLContext();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    log_windowdraw(_game, &_state->show_log, *_state);
    ImGui::ShowDemoWindow(&_state->show_demo_window);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
