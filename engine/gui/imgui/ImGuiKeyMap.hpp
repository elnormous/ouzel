#ifndef FLUXION_GUI_IMGUI_IMGUIKEYMAP_HPP
#define FLUXION_GUI_IMGUI_IMGUIKEYMAP_HPP

#include "imgui.h"
#include "../../input/Keyboard.hpp"
#include "../../input/Mouse.hpp"

namespace fluxion::gui::imgui
{
    // Translate a Fluxion keyboard key into the closest ImGuiKey.
    // Returns ImGuiKey_None for keys that have no Dear ImGui equivalent.
    ImGuiKey toImGuiKey(input::Keyboard::Key key) noexcept;

    // Translate a Fluxion mouse button into a Dear ImGui mouse-button index.
    // Returns -1 for buttons Dear ImGui doesn't track.
    int toImGuiMouseButton(input::Mouse::Button button) noexcept;
}

#endif // FLUXION_GUI_IMGUI_IMGUIKEYMAP_HPP
