#ifndef FLUXION_EDITOR_EDITORPICKING_HPP
#define FLUXION_EDITOR_EDITORPICKING_HPP

#include "../engine/math/Vector.hpp"

namespace fluxion::scene
{
    class Actor;
    class Camera;
    class Layer;
    class Scene;
}

namespace fluxion::editor
{
    // Pure editor-side picking and overlay helpers. No state — these
    // are free functions that are called once per click (pickActor) or
    // once per ImGui frame (drawAabbOverlay).
    //
    // Why editor-side and not engine::Scene::pickActor?
    //   * Scene::pickActor returns null for actors with no components,
    //     because Actor::pointOn iterates components and tests their
    //     bounding boxes. Bare actors (no SpriteRenderer / Mesh /
    //     PickShape attached) are invisible *and* unpickable to the
    //     engine. The editor needs them to be selectable so the user
    //     can attach components to them in the first place.
    //   * The editor falls back to a unit-cube AABB centred at the
    //     actor's local origin when getBoundingBox() is empty. This
    //     gives every actor a deterministic, transform-aware click
    //     target without polluting the runtime scene with editor-only
    //     pick shapes.

    // Project the actor's bounding box (or the editor-fallback unit
    // cube) into normalized window space and return the topmost actor
    // whose 2D screen-space AABB contains `normalizedPos`. Returns
    // nullptr if no actor is hit.
    //
    // `normalizedPos` is in the same coordinate system as Fluxion
    // MouseEvent::position — [0,1] across the window with origin top-
    // left.
    //
    // `excludeLayer`, if non-null, is skipped during traversal — used
    // by the editor to keep its internal helper layer (camera, future
    // gizmos) out of user-facing picking results.
    // `excludeActor` is similar but works at actor granularity, for
    // editor-internal helpers that have to live on a user layer (e.g.
    // a viewport camera that must see user content).
    scene::Actor* pickActor(scene::Scene& scene,
                            scene::Camera& camera,
                            const math::Vector<float, 2>& normalizedPos,
                            const scene::Layer* excludeLayer = nullptr,
                            const scene::Actor* excludeActor = nullptr);

    // Draws AABB outlines for every actor in the scene into a screen-
    // pixel rectangle, projecting through `camera`. Designed to be
    // called from inside an ImGui::Begin/End pair so the panel's
    // current window draw list receives the lines and they clip to
    // the panel's bounds.
    //
    // `rectMin`/`rectMax` are absolute ImGui screen coordinates
    // (typically `ImGui::GetItemRectMin/Max()` immediately after the
    // ImGui::Image call that hosts the viewport). The selected actor
    // is drawn brighter; the rest are dim. `excludeLayer` and
    // `excludeActor` mirror pickActor's filters.
    void drawAabbOverlay(scene::Scene& scene,
                         scene::Camera& camera,
                         scene::Actor* selectedActor,
                         const math::Vector<float, 2>& rectMin,
                         const math::Vector<float, 2>& rectMax,
                         const scene::Layer* excludeLayer = nullptr,
                         const scene::Actor* excludeActor = nullptr);
}

#endif // FLUXION_EDITOR_EDITORPICKING_HPP
