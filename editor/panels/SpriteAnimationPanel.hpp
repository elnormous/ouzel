#ifndef FLUXION_EDITOR_SPRITEANIMATIONPANEL_HPP
#define FLUXION_EDITOR_SPRITEANIMATIONPANEL_HPP

#include "../Panel.hpp"

namespace fluxion::editor
{
    class PlayControlPanel;
    class SelectionContext;

    // Shows the SpriteRenderer animation map of the currently
    // selected actor and lets the user tune each animation's
    // frameInterval, hit Play / Pause / Stop, and jump to a
    // specific frame. The Inspector already exposes a "Playing"
    // boolField for the parent SpriteRenderer, but it doesn't
    // surface the per-animation knobs — this panel does.
    //
    // 13b initial cut: text-only frame strip (clickable buttons
    // labelled "0", "1", ... that call setAnimationTime to jump);
    // a future iteration may render per-frame UV thumbnails using
    // the parent texture once SpriteData::Frame exposes its UV
    // rect. Drop-to-append-frame and Add/Remove animation are
    // also future work.
    class SpriteAnimationPanel final : public Panel
    {
    public:
        // playControl is optional but recommended: when provided,
        // the panel knows whether the editor is in Editing /
        // Playing / Paused state, and gates its edit-time manual
        // update() tick to Editing only — otherwise the manual
        // tick would keep animations advancing during Pause /
        // Step (both report engine.timeScale == 0 just like Edit
        // mode). Without playControl the panel falls back to a
        // pure timeScale check that's right for Edit but
        // incorrect for Pause.
        explicit SpriteAnimationPanel(SelectionContext* selection,
                                       const PlayControlPanel* playControl = nullptr) noexcept;

        void draw() override;

    private:
        SelectionContext* selection_;
        const PlayControlPanel* playControl_;
    };
}

#endif // FLUXION_EDITOR_SPRITEANIMATIONPANEL_HPP
