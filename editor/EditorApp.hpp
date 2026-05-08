#ifndef FLUXION_EDITOR_EDITORAPP_HPP
#define FLUXION_EDITOR_EDITORAPP_HPP

#include <memory>
#include <string>
#include <vector>
#include "../engine/core/Application.hpp"
#include "../engine/gui/imgui/ImGuiContext.hpp"
#include "Panel.hpp"

namespace fluxion::editor
{
    // The editor's Fluxion Application. Owns the ImGui Context, the panel
    // list, and (in future phases) the currently-edited Scene + selection.
    //
    // Cross-platform note: this class is desktop-only by design. The
    // engine-side foundations (ImGui integration, reflection, serializer)
    // build on every Fluxion-supported platform, but the editor itself
    // only ships for Windows / macOS / Linux. Mobile / web targets run
    // the user's game (samples / their own application), not the editor.
    class EditorApp final : public fluxion::Application
    {
    public:
        EditorApp();
        ~EditorApp() override;

        EditorApp(const EditorApp&) = delete;
        EditorApp& operator=(const EditorApp&) = delete;

    private:
        void registerPanels();
        std::string runReflectionSelfTest();
        std::string runSceneRoundTripSelfTest();

        gui::imgui::Context imgui_;
        std::vector<std::unique_ptr<Panel>> panels_;
    };
}

#endif // FLUXION_EDITOR_EDITORAPP_HPP
