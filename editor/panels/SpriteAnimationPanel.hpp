#ifndef FLUXION_EDITOR_SPRITEANIMATIONPANEL_HPP
#define FLUXION_EDITOR_SPRITEANIMATIONPANEL_HPP

#include "../Panel.hpp"

namespace fluxion::editor
{
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
        explicit SpriteAnimationPanel(SelectionContext* selection) noexcept;

        void draw() override;

    private:
        SelectionContext* selection_;
    };
}

#endif // FLUXION_EDITOR_SPRITEANIMATIONPANEL_HPP
