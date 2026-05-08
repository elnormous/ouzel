#include "InfoPanel.hpp"
#include "imgui.h"

namespace fluxion::editor
{
    InfoPanel::InfoPanel(std::string driverName,
                         std::string reflectionSelfTest,
                         std::string sceneSelfTest):
        Panel{"Info", true},
        driverName_{std::move(driverName)},
        reflectionSelfTest_{std::move(reflectionSelfTest)},
        sceneSelfTest_{std::move(sceneSelfTest)} {}

    void InfoPanel::draw()
    {
        ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(420, 200), ImGuiCond_FirstUseEver);
        ImGui::Begin(displayName_.c_str());
        const ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Renderer: %s", driverName_.c_str());
        ImGui::Text("Display:  %.0f x %.0f", io.DisplaySize.x, io.DisplaySize.y);
        const bool hasMouse = io.MousePos.x > -1.0e6F;
        if (hasMouse)
            ImGui::Text("Mouse:    %.0f, %.0f", io.MousePos.x, io.MousePos.y);
        else
            ImGui::TextUnformatted("Mouse:    (move pointer over window)");
        ImGui::Text("Frame %d  %.1f FPS  dt %.3f ms",
                    ImGui::GetFrameCount(), io.Framerate, io.DeltaTime * 1000.0F);
        ImGui::Text("WantCapture: kbd=%d mouse=%d",
                    io.WantCaptureKeyboard ? 1 : 0,
                    io.WantCaptureMouse ? 1 : 0);
        ImGui::Separator();
        ImGui::Text("Reflection self-test:    %s", reflectionSelfTest_.c_str());
        ImGui::Text("Scene round-trip self-test: %s", sceneSelfTest_.c_str());
        ImGui::End();
    }
}
