#include "SpriteAnimationPanel.hpp"

#include <cstdio>
#include "imgui.h"
#include "../SelectionContext.hpp"
#include "../../engine/core/Engine.hpp"
#include "../../engine/scene/Actor.hpp"
#include "../../engine/scene/SpriteRenderer.hpp"
#include "PlayControlPanel.hpp"

namespace fluxion::editor
{
    SpriteAnimationPanel::SpriteAnimationPanel(SelectionContext* selection,
                                                 const PlayControlPanel* playControl) noexcept:
        Panel{"Sprite Animation", true},
        selection_{selection},
        playControl_{playControl}
    {
    }

    void SpriteAnimationPanel::draw()
    {
        ImGui::SetNextWindowPos(ImVec2(720, 520), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(420, 220), ImGuiCond_FirstUseEver);
        ImGui::Begin(displayName_.c_str());

        scene::Actor* actor = selection_ ? selection_->getSelected() : nullptr;
        if (!actor)
        {
            ImGui::TextDisabled("(no actor selected)");
            ImGui::End();
            return;
        }

        // Find the actor's first SpriteRenderer. Most actors carry a
        // single one; if multiple exist, the first wins — a future
        // iteration could let the user pick which renderer to edit.
        scene::SpriteRenderer* sprite = nullptr;
        for (auto* comp : actor->getComponents())
        {
            if (auto* r = dynamic_cast<scene::SpriteRenderer*>(comp))
            {
                sprite = r;
                break;
            }
        }
        if (!sprite)
        {
            ImGui::TextDisabled("(selected actor has no SpriteRenderer)");
            ImGui::End();
            return;
        }

        auto& animations = sprite->getAnimationsMutable();
        if (animations.empty())
        {
            ImGui::TextDisabled("(no animations on this SpriteRenderer)");
            ImGui::TextDisabled("Drop a sprite asset onto the Sprite field");
            ImGui::TextDisabled("in the Inspector to populate.");
            ImGui::End();
            return;
        }

        // Top-level transport. Play does a full restart sequence
        // (stop + setAnimation with repeat from the current anim's
        // Loop flag + play) so the button keeps working after a
        // non-looping animation has played itself out —
        // SpriteRenderer::play() is a no-op when `playing` is
        // already true, which it stays after natural end, so a
        // plain play() click would do nothing.
        const bool isPlaying = sprite->isPlaying();
        if (ImGui::Button(isPlaying ? "Pause" : "Play"))
        {
            if (isPlaying)
            {
                sprite->stop(false);     // pause without resetting time
            }
            else
            {
                const auto* cur = sprite->getAnimation();
                const std::string name = cur ? cur->name : std::string{};
                const auto i = animations.find(name);
                const bool wantRepeat = (i != animations.end()) ? i->second.repeat : true;
                sprite->stop(true);      // ensure playing=false, time=0
                sprite->setAnimation(name, wantRepeat);
                sprite->play();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
            sprite->stop(true);
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
            sprite->reset();
        ImGui::Separator();

        // Edit-time tick. The PlayControlPanel sets engine.timeScale
        // to 0 in the default Editing state, which means the
        // dispatcher fires UpdateEvents with delta=0 — the
        // SpriteRenderer's own update() advances currentTime by 0,
        // so animation never visually progresses. To make the
        // panel's Play button useful in Editing mode, manually
        // drive update() here with the panel's real-time delta.
        //
        // Gating: Editing state only. Pause and Step also report
        // timeScale==0 (the engine's freeze mechanism), so a
        // pure timeScale check would override Pause and keep
        // animations running — exactly what the user noticed.
        // The PlayControlPanel back-pointer lets us tell Editing
        // from Paused. Falls back to the old timeScale check
        // when no playControl was wired (defensive default).
        const bool isEditing = playControl_
            ? (playControl_->state() == PlayControlPanel::State::Editing)
            : (engine->getTimeScale() == 0.0F);
        if (isEditing && sprite->isPlaying())
            sprite->update(ImGui::GetIO().DeltaTime);

        // Per-animation rows. The map is keyed by name; we walk it
        // alphabetically so the order matches whatever the user sees
        // in the Inspector's animation combo (when that lands).
        // Each row has the name, a frame interval slider, and a
        // text-based frame strip (one button per frame); clicking
        // jumps the renderer to that frame's start time.
        for (auto& [name, anim] : animations)
        {
            ImGui::PushID(name.c_str());

            // Header line: "<name>"  (current animation gets a green
            // marker so the user knows which is playing live).
            // sprite->getAnimation() is only safe to dereference
            // when isPlaying() is true (or after init() has been
            // called and a queue entry exists); the &&-chain
            // protects either way.
            const std::string displayName = name.empty() ? "(default)" : name;
            const bool isCurrent =
                sprite->isPlaying() &&
                sprite->getAnimation() &&
                sprite->getAnimation()->name == anim.name;
            ImGui::TextColored(isCurrent
                ? ImVec4(0.6F, 0.9F, 0.6F, 1.0F)
                : ImVec4(0.85F, 0.85F, 0.85F, 1.0F),
                "%s   (%zu frame%s)",
                displayName.c_str(),
                anim.frames.size(),
                anim.frames.size() == 1 ? "" : "s");

            // Frame interval slider — directly mutates the
            // SpriteData::Animation, since SpriteRenderer
            // owns the animations map by value.
            float interval = anim.frameInterval;
            if (ImGui::SliderFloat("Frame interval", &interval,
                                   0.01F, 1.0F, "%.3f s"))
                anim.frameInterval = interval;

            // Loop toggle. Stored on the Animation itself so it
            // survives across actor reselection and (future) scene
            // round-trip. The runtime QueuedAnimation::repeat is
            // what actually loops the playback; mutating this
            // checkbox while the animation is current re-applies
            // setAnimation so the change takes effect immediately.
            bool repeatCopy = anim.repeat;
            if (ImGui::Checkbox("Loop", &repeatCopy))
            {
                anim.repeat = repeatCopy;
                if (sprite->isPlaying())
                {
                    const auto* cur = sprite->getAnimation();
                    if (cur && cur->name == anim.name)
                        sprite->setAnimation(name, anim.repeat);
                }
            }

            // "Set as current" button — switches the playing
            // animation without restarting playback. Useful if the
            // SpriteRenderer has multiple named animations and
            // the user wants to see a different one.
            char setLabel[64];
            std::snprintf(setLabel, sizeof(setLabel),
                          "Set as current##%s", name.c_str());
            if (ImGui::Button(setLabel))
                sprite->setAnimation(name, anim.repeat);

            // Frame strip: tiny buttons labeled by index. Clicking
            // jumps the renderer to the start time of that frame
            // through setAnimationTime, matching how the engine's
            // own scrub APIs work.
            if (!anim.frames.empty())
            {
                ImGui::Text("Frames:");
                for (std::size_t i = 0; i < anim.frames.size(); ++i)
                {
                    if (i > 0) ImGui::SameLine();
                    char frameLbl[16];
                    std::snprintf(frameLbl, sizeof(frameLbl), "%zu", i);
                    if (ImGui::SmallButton(frameLbl))
                    {
                        sprite->setAnimation(name, /*repeat*/ true);
                        sprite->setAnimationTime(
                            static_cast<float>(i) * anim.frameInterval);
                        sprite->stop(false);   // pause on the chosen frame
                    }
                }
            }

            ImGui::Separator();
            ImGui::PopID();
        }

        ImGui::End();
    }
}
