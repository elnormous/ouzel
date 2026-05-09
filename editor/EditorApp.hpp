#ifndef FLUXION_EDITOR_EDITORAPP_HPP
#define FLUXION_EDITOR_EDITORAPP_HPP

#include <memory>
#include <string>
#include <vector>
#include "../engine/assets/Bundle.hpp"
#include "../engine/core/Application.hpp"
#include "../engine/gui/imgui/ImGuiContext.hpp"
#include "../engine/scene/Actor.hpp"
#include "../engine/scene/Camera.hpp"
#include "../engine/scene/Layer.hpp"
#include "../engine/scene/Scene.hpp"
#include "asset/AssetDatabase.hpp"
#include "CommandStack.hpp"
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
        std::string runAssetDatabaseSelfTest();
        std::string runAssetReferenceRoundTripSelfTest();

        // Main menu + file dialog plumbing (phase 7).
        void drawMainMenu();
        void drawFileDialog();
        void drawDiscardModal();
        void doNewScene();
        void doSaveScene();
        void doOpenScene();

        // Edit menu plumbing (phase 9).
        void drawEditMenu();

        enum class FileDialogMode { None, Save, Open };
        FileDialogMode fileDialogMode_ = FileDialogMode::None;

        // "Are you sure?" deferred-action machinery for unsaved
        // changes. The user kicks off a destructive flow (New /
        // Open / OS window close); when dirty_, we intercept and
        // queue the action via pendingDiscardAction_, then show
        // a modal in the next frame's draw. The modal's Discard
        // button executes the queued action; Cancel clears it.
        enum class PendingDiscard { None, NewScene, OpenScene, Close };
        PendingDiscard pendingDiscardAction_ = PendingDiscard::None;
        // Set true once the user has confirmed they want to close
        // an unsaved scene. The engine's close-confirm handler
        // returns true while this is set so the second WM_CLOSE
        // proceeds without re-prompting.
        bool userConfirmedClose_ = false;
        // Dirty state is derived each frame from the command-stack
        // delta against the last-save snapshot — see updateDirty().
        // Coarse but cheap: undo back to the saved state still
        // reads as dirty until the next save.
        std::size_t savedUndoCount_ = 0;
        bool dirty_ = false;
        void updateDirty();
        void executePendingDiscard();
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

        // Editor-owned bundle for project assets. Distinct from any
        // bundle the engine already created (e.g. samples), so the
        // editor can release its imports independently when the
        // project root changes. Cache is shared (engine-global), so
        // imports show up under cache.getX(name|id).
        assets::Bundle projectBundle_;

        // Editor-side asset metadata + .meta sidecar driver. Owns the
        // Uuid <-> path mapping for files inside the project root.
        AssetDatabase assetDatabase_;

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

        // Undo/redo history for editor mutations. All destructive
        // panel actions (add/delete actor, future component & field
        // changes) push commands here instead of mutating directly,
        // so Ctrl+Z / Ctrl+Y can step through them.
        CommandStack commandStack_;

        std::vector<std::unique_ptr<Panel>> panels_;

        // Non-owning back-pointer to the viewport panel inside
        // `panels_`. Lets the editor query the viewport camera-actor
        // when threading "exclude this from picking / tree / overlay"
        // through editor surfaces. Lifetime is governed by panels_.
        class ViewportPanel* viewportPanel_ = nullptr;

        // Non-owning back-pointer to the play-control panel so the
        // top toolbar (drawMainMenu) can drive the same Play /
        // Pause / Stop transitions the panel exposes. Lifetime is
        // governed by panels_.
        class PlayControlPanel* playControl_ = nullptr;

        // Last-frame play state snapshot, used by the per-frame
        // poll in setDrawCallback to detect Stop transitions and
        // clear the CommandStack when re-entering Editing — Play
        // Mode's snapshot/restore returns the scene to its pre-
        // Play state, but any commands the user pushed during
        // Play would still reference Play-only actor UUIDs. Set
        // to Editing on construction since that's the entry state.
        int lastPlayState_ = 0;     // PlayControlPanel::State as int
    };
}

#endif // FLUXION_EDITOR_EDITORAPP_HPP
