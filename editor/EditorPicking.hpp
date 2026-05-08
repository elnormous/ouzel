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
    scene::Actor* pickActor(scene::Scene& scene,
                            scene::Camera& camera,
                            const math::Vector<float, 2>& normalizedPos,
                            const scene::Layer* excludeLayer = nullptr);

    // Draws the AABB outlines for every actor in the scene as an
    // ImGui foreground overlay. The selected actor (if any) is drawn
    // in a brighter highlight colour; the rest are dim. Must be called
    // from the ImGui draw callback (render thread, between NewFrame
    // and Render). The display size used for projection comes from
    // ImGui::GetIO().DisplaySize, so the caller doesn't have to thread
    // it through.
    //
    // `excludeLayer` follows the same convention as pickActor: actors
    // on that layer are not decorated.
    void drawAabbOverlay(scene::Scene& scene,
                         scene::Camera& camera,
                         scene::Actor* selectedActor,
                         const scene::Layer* excludeLayer = nullptr);
}

#endif // FLUXION_EDITOR_EDITORPICKING_HPP
