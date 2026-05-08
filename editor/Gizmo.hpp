#ifndef FLUXION_EDITOR_GIZMO_HPP
#define FLUXION_EDITOR_GIZMO_HPP

#include "../engine/math/Vector.hpp"

namespace fluxion::scene
{
    class Actor;
    class Camera;
}

namespace fluxion::editor
{
    // Translate gizmo: three colour-coded axis handles drawn around
    // the selected actor. The user clicks a handle and drags to move
    // the actor along that axis. State is held by value inside the
    // Gizmo (drag axis + drag start snapshot) so reuse across frames
    // is straightforward — owners just call update() once per frame
    // from inside the viewport panel's ImGui::Begin/End pair.
    //
    // Cross-platform: uses only ImGui drawing/input APIs (the same
    // surface that drives the rest of the editor) and the engine's
    // Camera / Actor math, so it builds and runs identically on
    // D3D11, OpenGL, and Metal.
    //
    // Future phases:
    //   * 5c-2 will add a rotate gizmo (axis rings) sharing the same
    //     hover/drag plumbing.
    //   * 5c-3 will add scale handles.
    //   * Mode switching (translate/rotate/scale) will land alongside.
    class Gizmo
    {
    public:
        enum class Axis { None, X, Y, Z };

        // Per-frame call. Must run inside the viewport panel's
        // ImGui::Begin/End so GetWindowDrawList() resolves to the
        // panel's draw list.
        //
        // `imgMin`/`imgMax` are the absolute screen-pixel bounds of
        // the ImGui::Image that hosts the viewport (exactly what the
        // AABB overlay receives).
        //
        // Returns true when the gizmo is currently consuming the
        // mouse — either because it just started a drag this frame,
        // or because a drag is in progress. The caller MUST skip
        // pickActor()/selection-change logic when this returns true,
        // otherwise a click on a gizmo handle would also reselect
        // whatever's underneath.
        bool update(scene::Actor* selectedActor,
                    scene::Camera& camera,
                    const math::Vector<float, 2>& imgMin,
                    const math::Vector<float, 2>& imgMax);

    private:
        Axis dragAxis_ = Axis::None;
        math::Vector<float, 3> dragStartPosition_{};
        math::Vector<float, 2> dragStartMouse_{};
    };
}

#endif // FLUXION_EDITOR_GIZMO_HPP
