#ifndef FLUXION_EDITOR_EDITORAPP_HPP
#define FLUXION_EDITOR_EDITORAPP_HPP

#include <memory>
#include <string>
#include <vector>
#include "../engine/core/Application.hpp"
#include "../engine/events/EventHandler.hpp"
#include "../engine/gui/imgui/ImGuiContext.hpp"
#include "../engine/scene/Actor.hpp"
#include "../engine/scene/Camera.hpp"
#include "../engine/scene/Layer.hpp"
#include "../engine/scene/Scene.hpp"
#include "Panel.hpp"
#include "SelectionContext.hpp"

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
        void buildTestScene();
        std::string runReflectionSelfTest();
        std::string runSceneRoundTripSelfTest();
        std::string runPlayControlSelfTest();

        gui::imgui::Context imgui_;

        // The editor's working scene. Lives in the editor application
        // rather than going through SceneManager (which is for the game
        // runtime); phase 5+ will introduce a viewport panel that renders
        // this scene via Camera into a render target for embedded display.
        //
        // workingLayer_ holds the user-visible content (the actors the
        // editor exposes through the Scene tree, picking, etc.).
        // editorLayer_ holds purely-internal helpers (the editor
        // camera, future gizmos). Both live in the same Scene because
        // the engine's render pipeline traverses Scene -> Layers; but
        // editor UIs filter editorLayer_ out so those helpers never
        // appear in the hierarchy or get picked / serialized as scene
        // content.
        scene::Scene workingScene_;
        scene::Layer workingLayer_;
        scene::Layer editorLayer_;

        // Editor camera. Owns the per-frame back-buffer clear so the
        // ImGui pass on top renders against a stable background instead
        // of accumulating previous frames. Held by value so the editor
        // controls its lifetime and can keep the same camera across
        // Play/Stop snapshots without rebuilding.
        scene::Camera editorCamera_;
        scene::Actor editorCameraActor_;

        // Shared selection state. Each panel that needs the current
        // selection takes a pointer to this; when the SceneTreePanel
        // updates the selection the InspectorPanel sees it on the next
        // frame because both read the same instance.
        SelectionContext selection_;

        // Held EventHandler that listens for left-mouse-press to
        // implement click-to-pick on the back buffer. Picking is
        // skipped when ImGui has captured the mouse (the click is
        // over a panel), so panels and the viewport coexist cleanly.
        // EventHandler is a member rather than a local in the
        // constructor because addEventHandler stores a non-owning
        // reference that must outlive the addEventHandler call.
        EventHandler pickHandler_;

        std::vector<std::unique_ptr<Panel>> panels_;
    };
}

#endif // FLUXION_EDITOR_EDITORAPP_HPP
