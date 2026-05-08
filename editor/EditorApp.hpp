#ifndef FLUXION_EDITOR_EDITORAPP_HPP
#define FLUXION_EDITOR_EDITORAPP_HPP

#include <memory>
#include <string>
#include <vector>
#include "../engine/core/Application.hpp"
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

        // Main menu + file dialog plumbing (phase 7).
        void drawMainMenu();
        void drawFileDialog();
        void doNewScene();
        void doSaveScene();
        void doOpenScene();

        enum class FileDialogMode { None, Save, Open };
        FileDialogMode fileDialogMode_ = FileDialogMode::None;
        // Fixed-size path buffer is enough for the editor's purposes
        // (the engine's storage::Path itself takes a string-view-ish
        // input); MAX_PATH on Windows is 260 and POSIX is typically
        // 4 KiB, but real scene paths are far smaller.
        char fileDialogBuf_[512] = {};
        std::string currentScenePath_;
        std::string statusMessage_;
        // Render-thread display lifetime for the status line. Decremented
        // each frame so save/load confirmations fade after a few seconds.
        int statusFramesRemaining_ = 0;

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

        std::vector<std::unique_ptr<Panel>> panels_;

        // Non-owning back-pointer to the viewport panel inside
        // `panels_`. Lets the editor query the viewport camera-actor
        // when threading "exclude this from picking / tree / overlay"
        // through editor surfaces. Lifetime is governed by panels_.
        class ViewportPanel* viewportPanel_ = nullptr;
    };
}

#endif // FLUXION_EDITOR_EDITORAPP_HPP
