#ifndef FLUXION_EDITOR_PLAYCONTROLPANEL_HPP
#define FLUXION_EDITOR_PLAYCONTROLPANEL_HPP

#include <atomic>
#include "../Panel.hpp"
#include "../../engine/formats/Json.hpp"

namespace fluxion::scene
{
    class Scene;
}

namespace fluxion::editor
{
    // Edit / Play / Paused state machine for the editor's currently-edited
    // scene, with the Engine-side time scale and step-one-frame plumbing.
    //
    // Design intent:
    //   * Editing  — timeScale = 0 by convention (no simulation tick), the
    //                user freely manipulates the scene through the
    //                inspector. This is the entry state.
    //   * Playing  — timeScale = the slider value (default 1.0). The
    //                scene was snapshot to JSON the moment Play was hit;
    //                pressing Stop restores that snapshot exactly.
    //   * Paused   — timeScale = 0 mid-playthrough. Step button calls
    //                Engine::requestStep(1) to advance exactly one update
    //                tick. Resume returns to Playing at the slider value.
    //
    // Cross-platform: this panel is desktop-only by definition (it lives
    // in the editor binary), but the engine-side TimeScale / requestStep
    // primitives it drives are pure C++17 atomics and run on every
    // platform Fluxion supports — so a future runtime "freeze frame"
    // feature on mobile or web would reuse the same Engine API.
    class PlayControlPanel final : public Panel
    {
    public:
        enum class State { Editing, Playing, Paused };

        // The panel needs a back-pointer to the editor's working scene so
        // it can serialize/restore around play sessions. Caller (EditorApp)
        // ensures the pointer outlives the panel.
        explicit PlayControlPanel(scene::Scene* targetScene);

        void draw() override;

        [[nodiscard]] State state() const noexcept { return state_; }

        // Public state-machine entry points. The detailed Time
        // panel (this class' own draw) and the editor's top
        // toolbar (EditorApp::drawMainMenu) both drive the same
        // transitions through these — keeping the behaviour
        // identical no matter which control surface the user
        // touches. Each method is a no-op when called from a
        // state where the transition wouldn't make sense (e.g.,
        // Play while already Playing), so callers don't need to
        // gate their UI buttons themselves.
        void enterPlay();
        void enterPause();
        void enterResume();
        void enterStop();
        void stepOne();

    private:
        // Sync the engine window title to the current state so the
        // user sees a [PLAYING] / [PAUSED] / (empty) prefix even
        // without looking at any panel. Called from every state
        // transition.
        void updateWindowTitle();

        scene::Scene* scene_;     // not owned
        State state_ = State::Editing;
        float playbackSpeed_ = 1.0F;

        // Snapshot taken on Play, restored on Stop. Stored as a parsed
        // json::Value rather than the encoded string so restore is fast
        // and round-trip-stable (encode/parse adds float precision drift).
        json::Value snapshot_;
        bool hasSnapshot_ = false;

        std::string statusMessage_;
    };
}

#endif // FLUXION_EDITOR_PLAYCONTROLPANEL_HPP
