#ifndef IMGUIHOST_HPP
#define IMGUIHOST_HPP

#include <atomic>
#include "../engine/gui/imgui/ImGuiContext.hpp"

namespace samples
{
    // Shared state between samples/main.cpp (which owns the ImGui context and
    // the per-frame draw callback) and the ImGuiSample scene (which exposes
    // native Fluxion buttons that toggle ImGui windows on/off).
    //
    // The per-window flags are read on the render thread (inside the ImGui
    // draw callback) and written from the UI thread (inside the button click
    // handler), so they are std::atomic.
    struct ImGuiHost
    {
        std::atomic<bool> showSmokeTest{true};
        std::atomic<bool> showTextRef{true};
        std::atomic<bool> showDemo{true};
        std::atomic<bool> showReflection{true};

        // The Context's master switch is mirrored here only for convenience
        // (so the sample can toggle the button label between "Disable ImGui"
        // and "Enable ImGui"). The authoritative state lives on the Context.
        fluxion::gui::imgui::Context* context = nullptr;
    };

    ImGuiHost& imguiHost() noexcept;
}

#endif // IMGUIHOST_HPP
