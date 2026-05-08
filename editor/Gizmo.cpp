#include "Gizmo.hpp"

#include <array>
#include <cmath>
#include <limits>
#include "imgui.h"
#include "../engine/math/Constants.hpp"
#include "../engine/scene/Actor.hpp"
#include "../engine/scene/Camera.hpp"

namespace fluxion::editor
{
    namespace
    {
        // World-space length of each translate-arrow / radius of each
        // rotate-ring. Sized for the test scene's 60x60 ortho view —
        // visible without dominating it.
        constexpr float kAxisLengthWorld = 5.0F;
        constexpr float kRingRadiusWorld = 5.0F;

        // Pixel slack around a handle that still counts as a hit.
        // Generous enough to be friendly with mouse precision, tight
        // enough that adjacent handles don't fight over the same click.
        constexpr float kHitThreshold = 8.0F;

        // Minimum projected size below which an axis/ring is collapsed
        // and hidden — typical for a perpendicular axis in the
        // orthographic editor camera. Drawing/hit-testing a degenerate
        // handle would produce a confusing single-point target.
        constexpr float kMinAxisScreenLength = 6.0F;
        constexpr float kMinRingScreenExtent = 12.0F;

        constexpr int kRingSegments = 48;

        math::Vector<float, 2> projectToImageRect(
            scene::Camera& camera,
            const math::Vector<float, 3>& worldPos,
            const math::Vector<float, 2>& imgMin,
            const math::Vector<float, 2>& imgMax)
        {
            const auto normalized = camera.convertWorldToNormalized(worldPos);
            return math::Vector<float, 2>{
                imgMin.v[0] + normalized.v[0] * (imgMax.v[0] - imgMin.v[0]),
                imgMin.v[1] + normalized.v[1] * (imgMax.v[1] - imgMin.v[1])
            };
        }

        float distancePointToSegment(
            const math::Vector<float, 2>& p,
            const math::Vector<float, 2>& a,
            const math::Vector<float, 2>& b)
        {
            const float abx = b.v[0] - a.v[0];
            const float aby = b.v[1] - a.v[1];
            const float apx = p.v[0] - a.v[0];
            const float apy = p.v[1] - a.v[1];
            const float ab2 = abx * abx + aby * aby;
            if (ab2 <= 0.0F)
            {
                const float dx = p.v[0] - a.v[0];
                const float dy = p.v[1] - a.v[1];
                return std::sqrt(dx * dx + dy * dy);
            }
            float t = (apx * abx + apy * aby) / ab2;
            if (t < 0.0F) t = 0.0F;
            if (t > 1.0F) t = 1.0F;
            const float cx = a.v[0] + t * abx;
            const float cy = a.v[1] + t * aby;
            const float dx = p.v[0] - cx;
            const float dy = p.v[1] - cy;
            return std::sqrt(dx * dx + dy * dy);
        }

        struct AxisProjection
        {
            math::Vector<float, 2> end;
            float screenLength;
            bool visible;
        };

        AxisProjection projectAxis(
            scene::Camera& camera,
            const math::Vector<float, 3>& worldEnd,
            const math::Vector<float, 2>& screenOrigin,
            const math::Vector<float, 2>& imgMin,
            const math::Vector<float, 2>& imgMax)
        {
            const auto end = projectToImageRect(camera, worldEnd, imgMin, imgMax);
            const float dx = end.v[0] - screenOrigin.v[0];
            const float dy = end.v[1] - screenOrigin.v[1];
            const float length = std::sqrt(dx * dx + dy * dy);
            return AxisProjection{end, length,
                                  length >= kMinAxisScreenLength};
        }

        // Build the ring of `kRingSegments` projected screen points
        // for an axis. The ring lies in the plane spanned by `u` and
        // `v` (both unit world vectors perpendicular to the axis),
        // centred on `actorPos`.
        struct RingProjection
        {
            std::array<math::Vector<float, 2>, kRingSegments> points;
            float screenWidth;
            float screenHeight;
            bool visible;
        };

        RingProjection projectRing(
            scene::Camera& camera,
            const math::Vector<float, 3>& actorPos,
            const math::Vector<float, 3>& u,
            const math::Vector<float, 3>& v,
            const math::Vector<float, 2>& imgMin,
            const math::Vector<float, 2>& imgMax)
        {
            RingProjection r{};
            float minX = std::numeric_limits<float>::max();
            float minY = std::numeric_limits<float>::max();
            float maxX = std::numeric_limits<float>::lowest();
            float maxY = std::numeric_limits<float>::lowest();
            for (int i = 0; i < kRingSegments; ++i)
            {
                const float angle =
                    static_cast<float>(i) * math::tau<float> /
                    static_cast<float>(kRingSegments);
                const float c = std::cos(angle) * kRingRadiusWorld;
                const float s = std::sin(angle) * kRingRadiusWorld;
                const auto worldP = math::Vector<float, 3>{
                    actorPos.v[0] + u.v[0] * c + v.v[0] * s,
                    actorPos.v[1] + u.v[1] * c + v.v[1] * s,
                    actorPos.v[2] + u.v[2] * c + v.v[2] * s
                };
                r.points[i] = projectToImageRect(camera, worldP, imgMin, imgMax);
                if (r.points[i].v[0] < minX) minX = r.points[i].v[0];
                if (r.points[i].v[1] < minY) minY = r.points[i].v[1];
                if (r.points[i].v[0] > maxX) maxX = r.points[i].v[0];
                if (r.points[i].v[1] > maxY) maxY = r.points[i].v[1];
            }
            r.screenWidth = maxX - minX;
            r.screenHeight = maxY - minY;
            // A ring needs SOME extent in BOTH axes to be a useful
            // click target — otherwise it has collapsed to a line.
            r.visible = r.screenWidth >= kMinRingScreenExtent &&
                        r.screenHeight >= kMinRingScreenExtent;
            return r;
        }

        // Closest distance from `mouse` to any segment of the ring
        // (treated as a closed polyline).
        float distancePointToRing(
            const math::Vector<float, 2>& mouse,
            const RingProjection& ring)
        {
            float best = std::numeric_limits<float>::max();
            for (int i = 0; i < kRingSegments; ++i)
            {
                const int j = (i + 1) % kRingSegments;
                const float d = distancePointToSegment(
                    mouse, ring.points[i], ring.points[j]);
                if (d < best) best = d;
            }
            return best;
        }

        // Wrap an angle delta into (-pi, pi]. Used so the rotate drag
        // tracks naturally across the +/-pi atan2 boundary.
        float wrapToPi(float a) noexcept
        {
            const float twoPi = math::tau<float>;
            while (a > math::pi<float>) a -= twoPi;
            while (a <= -math::pi<float>) a += twoPi;
            return a;
        }

        // Three basis vectors used for axis directions in the gizmo.
        // World mode = identity basis. Local mode = the actor's
        // rotated basis, so handles follow the actor's orientation.
        struct AxisBasis
        {
            math::Vector<float, 3> x, y, z;
        };

        AxisBasis computeBasis(bool useLocal,
                               const math::Quaternion<float>& rotation)
        {
            if (useLocal)
                return AxisBasis{
                    math::getRightVector(rotation),
                    math::getUpVector(rotation),
                    math::getForwardVector(rotation)};
            return AxisBasis{
                math::Vector<float, 3>{1.0F, 0.0F, 0.0F},
                math::Vector<float, 3>{0.0F, 1.0F, 0.0F},
                math::Vector<float, 3>{0.0F, 0.0F, 1.0F}};
        }
    }

    bool Gizmo::update(scene::Actor* selectedActor,
                      scene::Camera& camera,
                      const math::Vector<float, 2>& imgMin,
                      const math::Vector<float, 2>& imgMax)
    {
        if (!selectedActor)
        {
            cancelDrag();
            return false;
        }

        switch (mode_)
        {
            case Mode::Translate:
                return updateTranslate(*selectedActor, camera, imgMin, imgMax);
            case Mode::Rotate:
                return updateRotate(*selectedActor, camera, imgMin, imgMax);
            case Mode::Scale:
                return updateScale(*selectedActor, camera, imgMin, imgMax);
        }
        return false;
    }

    bool Gizmo::updateTranslate(scene::Actor& actor,
                               scene::Camera& camera,
                               const math::Vector<float, 2>& imgMin,
                               const math::Vector<float, 2>& imgMax)
    {
        const auto pos = actor.getPosition();
        const auto basis = computeBasis(space_ == Space::Local,
                                        actor.getRotation());
        const auto sPos = projectToImageRect(camera, pos, imgMin, imgMax);
        const auto axisX = projectAxis(camera,
            math::Vector<float, 3>{
                pos.v[0] + basis.x.v[0] * kAxisLengthWorld,
                pos.v[1] + basis.x.v[1] * kAxisLengthWorld,
                pos.v[2] + basis.x.v[2] * kAxisLengthWorld},
            sPos, imgMin, imgMax);
        const auto axisY = projectAxis(camera,
            math::Vector<float, 3>{
                pos.v[0] + basis.y.v[0] * kAxisLengthWorld,
                pos.v[1] + basis.y.v[1] * kAxisLengthWorld,
                pos.v[2] + basis.y.v[2] * kAxisLengthWorld},
            sPos, imgMin, imgMax);
        const auto axisZ = projectAxis(camera,
            math::Vector<float, 3>{
                pos.v[0] + basis.z.v[0] * kAxisLengthWorld,
                pos.v[1] + basis.z.v[1] * kAxisLengthWorld,
                pos.v[2] + basis.z.v[2] * kAxisLengthWorld},
            sPos, imgMin, imgMax);

        const ImVec2 mouse = ImGui::GetMousePos();
        const math::Vector<float, 2> mousePos{mouse.x, mouse.y};
        const bool mouseInImg =
            mouse.x >= imgMin.v[0] && mouse.x <= imgMax.v[0] &&
            mouse.y >= imgMin.v[1] && mouse.y <= imgMax.v[1];

        Axis hovered = Axis::None;
        if (dragAxis_ == Axis::None && mouseInImg)
        {
            float best = kHitThreshold;
            if (axisX.visible)
            {
                const float d = distancePointToSegment(mousePos, sPos, axisX.end);
                if (d < best) { best = d; hovered = Axis::X; }
            }
            if (axisY.visible)
            {
                const float d = distancePointToSegment(mousePos, sPos, axisY.end);
                if (d < best) { best = d; hovered = Axis::Y; }
            }
            if (axisZ.visible)
            {
                const float d = distancePointToSegment(mousePos, sPos, axisZ.end);
                if (d < best) { best = d; hovered = Axis::Z; }
            }
        }

        bool claimedMouse = false;
        if (dragAxis_ == Axis::None)
        {
            if (hovered != Axis::None &&
                ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                dragAxis_ = hovered;
                dragStartPosition_ = pos;
                dragStartMouse_ = mousePos;
                claimedMouse = true;
            }
        }
        else
        {
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                dragAxis_ = Axis::None;
            }
            else
            {
                math::Vector<float, 3> axisWorld{};
                math::Vector<float, 2> axisScreen{};
                switch (dragAxis_)
                {
                    case Axis::X:
                        axisWorld = basis.x;
                        axisScreen = math::Vector<float, 2>{
                            axisX.end.v[0] - sPos.v[0],
                            axisX.end.v[1] - sPos.v[1]};
                        break;
                    case Axis::Y:
                        axisWorld = basis.y;
                        axisScreen = math::Vector<float, 2>{
                            axisY.end.v[0] - sPos.v[0],
                            axisY.end.v[1] - sPos.v[1]};
                        break;
                    case Axis::Z:
                        axisWorld = basis.z;
                        axisScreen = math::Vector<float, 2>{
                            axisZ.end.v[0] - sPos.v[0],
                            axisZ.end.v[1] - sPos.v[1]};
                        break;
                    case Axis::None: break;
                }

                const float screenLen2 =
                    axisScreen.v[0] * axisScreen.v[0] +
                    axisScreen.v[1] * axisScreen.v[1];
                if (screenLen2 > 1e-6F)
                {
                    const float mdx = mousePos.v[0] - dragStartMouse_.v[0];
                    const float mdy = mousePos.v[1] - dragStartMouse_.v[1];
                    const float dot =
                        mdx * axisScreen.v[0] + mdy * axisScreen.v[1];
                    const float t = dot / screenLen2 * kAxisLengthWorld;
                    actor.setPosition(math::Vector<float, 3>{
                        dragStartPosition_.v[0] + axisWorld.v[0] * t,
                        dragStartPosition_.v[1] + axisWorld.v[1] * t,
                        dragStartPosition_.v[2] + axisWorld.v[2] * t
                    });
                }
                claimedMouse = true;
            }
        }

        auto* drawList = ImGui::GetWindowDrawList();
        if (drawList)
        {
            drawList->PushClipRect(
                ImVec2{imgMin.v[0], imgMin.v[1]},
                ImVec2{imgMax.v[0], imgMax.v[1]}, true);
            const ImU32 colX = IM_COL32(220, 60, 60, 230);
            const ImU32 colY = IM_COL32(60, 220, 60, 230);
            const ImU32 colZ = IM_COL32(80, 120, 240, 230);
            const ImU32 colHi = IM_COL32(255, 220, 80, 255);
            auto pickColor = [&](Axis a, ImU32 base) {
                return (a == hovered || a == dragAxis_) ? colHi : base;
            };
            const float thickness = 3.0F;
            const ImVec2 origin{sPos.v[0], sPos.v[1]};
            if (axisX.visible)
                drawList->AddLine(origin,
                                  ImVec2{axisX.end.v[0], axisX.end.v[1]},
                                  pickColor(Axis::X, colX), thickness);
            if (axisY.visible)
                drawList->AddLine(origin,
                                  ImVec2{axisY.end.v[0], axisY.end.v[1]},
                                  pickColor(Axis::Y, colY), thickness);
            if (axisZ.visible)
                drawList->AddLine(origin,
                                  ImVec2{axisZ.end.v[0], axisZ.end.v[1]},
                                  pickColor(Axis::Z, colZ), thickness);
            drawList->AddCircleFilled(origin, 4.0F,
                                      IM_COL32(255, 255, 255, 220));
            drawList->PopClipRect();
        }

        return claimedMouse;
    }

    bool Gizmo::updateRotate(scene::Actor& actor,
                            scene::Camera& camera,
                            const math::Vector<float, 2>& imgMin,
                            const math::Vector<float, 2>& imgMax)
    {
        const auto pos = actor.getPosition();
        const auto basis = computeBasis(space_ == Space::Local,
                                        actor.getRotation());
        const auto sPos = projectToImageRect(camera, pos, imgMin, imgMax);

        // Each ring spans the plane perpendicular to its axis. World
        // mode = world-aligned rings; Local mode = rings tilted with
        // the actor so they line up with the actor's own axes.
        const auto ringX = projectRing(camera, pos,
            basis.y, basis.z, imgMin, imgMax);
        const auto ringY = projectRing(camera, pos,
            basis.x, basis.z, imgMin, imgMax);
        const auto ringZ = projectRing(camera, pos,
            basis.x, basis.y, imgMin, imgMax);

        const ImVec2 mouse = ImGui::GetMousePos();
        const math::Vector<float, 2> mousePos{mouse.x, mouse.y};
        const bool mouseInImg =
            mouse.x >= imgMin.v[0] && mouse.x <= imgMax.v[0] &&
            mouse.y >= imgMin.v[1] && mouse.y <= imgMax.v[1];

        Axis hovered = Axis::None;
        if (dragAxis_ == Axis::None && mouseInImg)
        {
            float best = kHitThreshold;
            if (ringX.visible)
            {
                const float d = distancePointToRing(mousePos, ringX);
                if (d < best) { best = d; hovered = Axis::X; }
            }
            if (ringY.visible)
            {
                const float d = distancePointToRing(mousePos, ringY);
                if (d < best) { best = d; hovered = Axis::Y; }
            }
            if (ringZ.visible)
            {
                const float d = distancePointToRing(mousePos, ringZ);
                if (d < best) { best = d; hovered = Axis::Z; }
            }
        }

        // Screen-plane angle from the actor pivot to the cursor.
        // Used both to seed the drag and to track it. Approximates
        // the rotation around any axis well in the orthographic
        // editor view; a perspective viewport would want a per-axis
        // projected basis, but that's a refinement for later.
        const float angleNow = std::atan2(mousePos.v[1] - sPos.v[1],
                                          mousePos.v[0] - sPos.v[0]);

        bool claimedMouse = false;
        if (dragAxis_ == Axis::None)
        {
            if (hovered != Axis::None &&
                ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                dragAxis_ = hovered;
                dragStartRotation_ = actor.getRotation();
                prevAngle_ = angleNow;
                accumulatedAngle_ = 0.0F;
                claimedMouse = true;
            }
        }
        else
        {
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                dragAxis_ = Axis::None;
            }
            else
            {
                accumulatedAngle_ += wrapToPi(angleNow - prevAngle_);
                prevAngle_ = angleNow;

                math::Vector<float, 3> axisDir{};
                switch (dragAxis_)
                {
                    case Axis::X:
                        axisDir = math::Vector<float, 3>{1.0F, 0.0F, 0.0F};
                        break;
                    case Axis::Y:
                        axisDir = math::Vector<float, 3>{0.0F, 1.0F, 0.0F};
                        break;
                    case Axis::Z:
                        axisDir = math::Vector<float, 3>{0.0F, 0.0F, 1.0F};
                        break;
                    case Axis::None: break;
                }

                // Sign convention: ImGui screen-Y grows downward, so a
                // visually-clockwise drag yields a positive atan2
                // delta. The engine's editor camera looks down +Z (so
                // +Z points INTO the screen from the viewer), which
                // means a right-handed positive rotation around the
                // axis is mathematically counterclockwise viewed from
                // +axis — but the viewer sits at -Z, so they see the
                // OPPOSITE handedness. Negating the accumulated angle
                // here keeps the cursor direction aligned with the
                // visible rotation direction for every axis.
                math::Quaternion<float> deltaQuat;
                math::setRotation(deltaQuat, -accumulatedAngle_, axisDir);
                // World mode = pre-multiply: rotate around the world
                // axis on top of the actor's start orientation.
                // Local mode = post-multiply: the rotation happens in
                // the LOCAL frame established by startRotation, so
                // axis (1,0,0) is the actor's local X regardless of
                // its current orientation. Both are textbook quaternion
                // composition rules.
                if (space_ == Space::Local)
                    actor.setRotation(dragStartRotation_ * deltaQuat);
                else
                    actor.setRotation(deltaQuat * dragStartRotation_);
                claimedMouse = true;
            }
        }

        auto* drawList = ImGui::GetWindowDrawList();
        if (drawList)
        {
            drawList->PushClipRect(
                ImVec2{imgMin.v[0], imgMin.v[1]},
                ImVec2{imgMax.v[0], imgMax.v[1]}, true);

            const ImU32 colX = IM_COL32(220, 60, 60, 230);
            const ImU32 colY = IM_COL32(60, 220, 60, 230);
            const ImU32 colZ = IM_COL32(80, 120, 240, 230);
            const ImU32 colHi = IM_COL32(255, 220, 80, 255);
            auto pickColor = [&](Axis a, ImU32 base) {
                return (a == hovered || a == dragAxis_) ? colHi : base;
            };

            auto drawRing = [&](const RingProjection& r,
                                Axis axis, ImU32 base) {
                if (!r.visible) return;
                const ImU32 c = pickColor(axis, base);
                std::array<ImVec2, kRingSegments> pts;
                for (int i = 0; i < kRingSegments; ++i)
                    pts[i] = ImVec2{r.points[i].v[0], r.points[i].v[1]};
                drawList->AddPolyline(pts.data(), kRingSegments, c,
                                      ImDrawFlags_Closed,
                                      (axis == hovered || axis == dragAxis_)
                                          ? 3.0F : 2.0F);
            };

            drawRing(ringX, Axis::X, colX);
            drawRing(ringY, Axis::Y, colY);
            drawRing(ringZ, Axis::Z, colZ);

            drawList->AddCircleFilled(ImVec2{sPos.v[0], sPos.v[1]},
                                      4.0F,
                                      IM_COL32(255, 255, 255, 220));
            drawList->PopClipRect();
        }

        return claimedMouse;
    }

    bool Gizmo::updateScale(scene::Actor& actor,
                           scene::Camera& camera,
                           const math::Vector<float, 2>& imgMin,
                           const math::Vector<float, 2>& imgMax)
    {
        const auto pos = actor.getPosition();
        // Scale is per-component on the actor's own scale vector, so
        // the handles always live on the actor's local axes — even
        // when the gizmo Space is set to World for the other modes.
        // Visually showing world-aligned handles for scale while the
        // numbers modify local components would mismatch and confuse.
        const auto basis = computeBasis(/*useLocal*/ true,
                                        actor.getRotation());
        const auto sPos = projectToImageRect(camera, pos, imgMin, imgMax);
        const auto axisX = projectAxis(camera,
            math::Vector<float, 3>{
                pos.v[0] + basis.x.v[0] * kAxisLengthWorld,
                pos.v[1] + basis.x.v[1] * kAxisLengthWorld,
                pos.v[2] + basis.x.v[2] * kAxisLengthWorld},
            sPos, imgMin, imgMax);
        const auto axisY = projectAxis(camera,
            math::Vector<float, 3>{
                pos.v[0] + basis.y.v[0] * kAxisLengthWorld,
                pos.v[1] + basis.y.v[1] * kAxisLengthWorld,
                pos.v[2] + basis.y.v[2] * kAxisLengthWorld},
            sPos, imgMin, imgMax);
        const auto axisZ = projectAxis(camera,
            math::Vector<float, 3>{
                pos.v[0] + basis.z.v[0] * kAxisLengthWorld,
                pos.v[1] + basis.z.v[1] * kAxisLengthWorld,
                pos.v[2] + basis.z.v[2] * kAxisLengthWorld},
            sPos, imgMin, imgMax);

        const ImVec2 mouse = ImGui::GetMousePos();
        const math::Vector<float, 2> mousePos{mouse.x, mouse.y};
        const bool mouseInImg =
            mouse.x >= imgMin.v[0] && mouse.x <= imgMax.v[0] &&
            mouse.y >= imgMin.v[1] && mouse.y <= imgMax.v[1];

        // Same hit test as translate — the scale gizmo's hot region is
        // the axis line, just terminated with a square box rather than
        // an arrow.
        Axis hovered = Axis::None;
        if (dragAxis_ == Axis::None && mouseInImg)
        {
            float best = kHitThreshold;
            if (axisX.visible)
            {
                const float d = distancePointToSegment(mousePos, sPos, axisX.end);
                if (d < best) { best = d; hovered = Axis::X; }
            }
            if (axisY.visible)
            {
                const float d = distancePointToSegment(mousePos, sPos, axisY.end);
                if (d < best) { best = d; hovered = Axis::Y; }
            }
            if (axisZ.visible)
            {
                const float d = distancePointToSegment(mousePos, sPos, axisZ.end);
                if (d < best) { best = d; hovered = Axis::Z; }
            }
        }

        // Project a screen-pixel point onto the chosen axis's screen
        // direction, returning the signed distance from `sPos` along
        // it. Used at click-time to anchor the drag, and per-frame to
        // compute the scale ratio against that anchor.
        auto projectOnAxisScreen = [&](Axis a, const math::Vector<float, 2>& p) -> float {
            math::Vector<float, 2> axisVec{};
            switch (a)
            {
                case Axis::X: axisVec = math::Vector<float, 2>{
                    axisX.end.v[0] - sPos.v[0],
                    axisX.end.v[1] - sPos.v[1]}; break;
                case Axis::Y: axisVec = math::Vector<float, 2>{
                    axisY.end.v[0] - sPos.v[0],
                    axisY.end.v[1] - sPos.v[1]}; break;
                case Axis::Z: axisVec = math::Vector<float, 2>{
                    axisZ.end.v[0] - sPos.v[0],
                    axisZ.end.v[1] - sPos.v[1]}; break;
                case Axis::None: break;
            }
            const float len2 =
                axisVec.v[0] * axisVec.v[0] + axisVec.v[1] * axisVec.v[1];
            if (len2 <= 1e-6F) return 0.0F;
            const float dx = p.v[0] - sPos.v[0];
            const float dy = p.v[1] - sPos.v[1];
            // Dot product / length — signed distance in pixels.
            return (dx * axisVec.v[0] + dy * axisVec.v[1]) / std::sqrt(len2);
        };

        bool claimedMouse = false;
        if (dragAxis_ == Axis::None)
        {
            if (hovered != Axis::None &&
                ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                dragAxis_ = hovered;
                dragStartScale_ = actor.getScale();
                dragStartAxisProjection_ =
                    projectOnAxisScreen(dragAxis_, mousePos);
                claimedMouse = true;
            }
        }
        else
        {
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                dragAxis_ = Axis::None;
            }
            else
            {
                // Ratio = current axis-projection / start projection.
                // When the cursor is at the same point the user clicked
                // (ratio == 1), scale is unchanged. Pulling away from
                // the pivot (greater projection) grows; pushing through
                // the pivot to the other side flips the sign — useful
                // for mirroring and intentionally allowed.
                const float now = projectOnAxisScreen(dragAxis_, mousePos);
                if (std::fabs(dragStartAxisProjection_) > 1e-3F)
                {
                    const float ratio = now / dragStartAxisProjection_;
                    auto newScale = dragStartScale_;
                    switch (dragAxis_)
                    {
                        case Axis::X: newScale.v[0] *= ratio; break;
                        case Axis::Y: newScale.v[1] *= ratio; break;
                        case Axis::Z: newScale.v[2] *= ratio; break;
                        case Axis::None: break;
                    }
                    actor.setScale(newScale);
                }
                claimedMouse = true;
            }
        }

        auto* drawList = ImGui::GetWindowDrawList();
        if (drawList)
        {
            drawList->PushClipRect(
                ImVec2{imgMin.v[0], imgMin.v[1]},
                ImVec2{imgMax.v[0], imgMax.v[1]}, true);
            const ImU32 colX = IM_COL32(220, 60, 60, 230);
            const ImU32 colY = IM_COL32(60, 220, 60, 230);
            const ImU32 colZ = IM_COL32(80, 120, 240, 230);
            const ImU32 colHi = IM_COL32(255, 220, 80, 255);
            auto pickColor = [&](Axis a, ImU32 base) {
                return (a == hovered || a == dragAxis_) ? colHi : base;
            };
            const float thickness = 3.0F;
            const ImVec2 origin{sPos.v[0], sPos.v[1]};

            // Square handle at the end of each axis — the visual cue
            // that this is scale (vs. translate's arrow). 5 pixel half-
            // size so it's a clear click target without dominating.
            constexpr float kBoxHalf = 5.0F;
            auto drawAxis = [&](const AxisProjection& a, Axis which, ImU32 base) {
                if (!a.visible) return;
                const ImU32 c = pickColor(which, base);
                drawList->AddLine(origin,
                                  ImVec2{a.end.v[0], a.end.v[1]},
                                  c, thickness);
                drawList->AddRectFilled(
                    ImVec2{a.end.v[0] - kBoxHalf, a.end.v[1] - kBoxHalf},
                    ImVec2{a.end.v[0] + kBoxHalf, a.end.v[1] + kBoxHalf},
                    c);
            };
            drawAxis(axisX, Axis::X, colX);
            drawAxis(axisY, Axis::Y, colY);
            drawAxis(axisZ, Axis::Z, colZ);

            drawList->AddCircleFilled(origin, 4.0F,
                                      IM_COL32(255, 255, 255, 220));
            drawList->PopClipRect();
        }

        return claimedMouse;
    }
}
