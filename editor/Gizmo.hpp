#ifndef FLUXION_EDITOR_GIZMO_HPP
#define FLUXION_EDITOR_GIZMO_HPP

#include "../engine/math/Quaternion.hpp"
#include "../engine/math/Vector.hpp"

namespace fluxion::scene
{
    class Actor;
    class Camera;
}

namespace fluxion::editor
{
    // Transform manipulator drawn around the selected actor inside
    // the viewport panel. Supports a mode-switch between translate
    // (axis arrows) and rotate (axis rings); scale will join in 5c-3.
    //
    // State is held by value: mode + drag snapshot. Owners just call
    // update() once per frame from inside the viewport panel's
    // ImGui::Begin/End pair.
    //
    // Cross-platform: uses only ImGui drawing/input APIs and the
    // engine's Camera / Actor / Quaternion math, so it builds and
    // runs identically on D3D11, OpenGL, and Metal.
    class Gizmo
    {
    public:
        enum class Mode { Translate, Rotate, Scale };
        enum class Axis { None, X, Y, Z };

        // Reference frame for the gizmo handles:
        //   * World — handles align with world XYZ. Translate moves
        //     along world axes; rotate spins around world axes.
        //   * Local — handles align with the actor's rotated basis.
        //     Translate moves along the actor's local axes; rotate
        //     spins around them. Scale always uses the actor's local
        //     basis because scale itself is component-wise on the
        //     actor's own axes.
        enum class Space { World, Local };

        void setMode(Mode m) noexcept { mode_ = m; cancelDrag(); }
        [[nodiscard]] Mode getMode() const noexcept { return mode_; }

        void setSpace(Space s) noexcept { space_ = s; cancelDrag(); }
        [[nodiscard]] Space getSpace() const noexcept { return space_; }

        // Per-frame call. Must run inside the viewport panel's
        // ImGui::Begin/End so GetWindowDrawList() resolves to the
        // panel's draw list. `imgMin`/`imgMax` are the absolute
        // screen-pixel bounds of the ImGui::Image rect.
        //
        // Returns true when the gizmo is currently consuming the
        // mouse — caller MUST skip pickActor() / selection updates.
        bool update(scene::Actor* selectedActor,
                    scene::Camera& camera,
                    const math::Vector<float, 2>& imgMin,
                    const math::Vector<float, 2>& imgMax);

    private:
        bool updateTranslate(scene::Actor& actor,
                             scene::Camera& camera,
                             const math::Vector<float, 2>& imgMin,
                             const math::Vector<float, 2>& imgMax);

        bool updateRotate(scene::Actor& actor,
                          scene::Camera& camera,
                          const math::Vector<float, 2>& imgMin,
                          const math::Vector<float, 2>& imgMax);

        bool updateScale(scene::Actor& actor,
                         scene::Camera& camera,
                         const math::Vector<float, 2>& imgMin,
                         const math::Vector<float, 2>& imgMax);

        // Resets drag state so a mode change or a null actor doesn't
        // leave a half-finished drag in flight.
        void cancelDrag() noexcept
        {
            dragAxis_ = Axis::None;
        }

        Mode mode_ = Mode::Translate;
        Space space_ = Space::World;
        Axis dragAxis_ = Axis::None;

        // Translate drag snapshot.
        math::Vector<float, 3> dragStartPosition_{};
        math::Vector<float, 2> dragStartMouse_{};

        // Scale drag snapshot. Captures the actor's scale and the
        // mouse-projection-along-axis at click time so the live drag
        // can compute a stable ratio against it.
        math::Vector<float, 3> dragStartScale_{};
        float dragStartAxisProjection_ = 0.0F;

        // Rotate drag snapshot. `prevAngle_` + `accumulatedAngle_`
        // together let drag track the cursor across the +/-pi atan2
        // wrap when the user circles the pivot multiple times: each
        // frame's delta is normalized into (-pi, pi] and accumulated.
        math::Quaternion<float> dragStartRotation_{};
        float prevAngle_ = 0.0F;
        float accumulatedAngle_ = 0.0F;
    };
}

#endif // FLUXION_EDITOR_GIZMO_HPP
