#include "Gizmo.hpp"

#include <cmath>
#include "imgui.h"
#include "../engine/scene/Actor.hpp"
#include "../engine/scene/Camera.hpp"

namespace fluxion::editor
{
    namespace
    {
        // World-space length of each axis arrow. Sized for the test
        // scene's 60x60 ortho view — visible without dominating it.
        constexpr float kAxisLengthWorld = 5.0F;

        // Pixel slack around the axis line that still counts as a hit.
        // Generous enough to be friendly with mouse precision but tight
        // enough that two adjacent axes don't fight over the same click.
        constexpr float kHitThreshold = 8.0F;

        // Minimum projected length (in pixels) below which an axis is
        // collapsed and hidden — typical for the Z axis in the
        // orthographic editor camera that looks down +Z. Drawing /
        // hit-testing a degenerate axis would produce a confusing
        // single-point handle the user can't really aim at.
        constexpr float kMinAxisScreenLength = 6.0F;

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

        // Closest distance from point `p` to the segment `a..b`, all in
        // 2D screen pixels. Returns the same large value for both
        // endpoints when the segment collapses to a point — the caller
        // filters those segments out via kMinAxisScreenLength so this
        // never matters in practice.
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
            const math::Vector<float, 3>& origin,
            const math::Vector<float, 3>& worldEnd,
            const math::Vector<float, 2>& screenOrigin,
            const math::Vector<float, 2>& imgMin,
            const math::Vector<float, 2>& imgMax)
        {
            (void)origin;
            const auto end = projectToImageRect(camera, worldEnd, imgMin, imgMax);
            const float dx = end.v[0] - screenOrigin.v[0];
            const float dy = end.v[1] - screenOrigin.v[1];
            const float length = std::sqrt(dx * dx + dy * dy);
            return AxisProjection{end, length, length >= kMinAxisScreenLength};
        }
    }

    bool Gizmo::update(scene::Actor* selectedActor,
                      scene::Camera& camera,
                      const math::Vector<float, 2>& imgMin,
                      const math::Vector<float, 2>& imgMax)
    {
        if (!selectedActor)
        {
            dragAxis_ = Axis::None;
            return false;
        }

        const auto pos = selectedActor->getPosition();

        // Project the actor origin and the three axis endpoints to
        // screen pixels through the viewport camera. We do this every
        // frame (no caching) because the actor's transform may have
        // changed via the Inspector, undo, or — once a drag is in
        // progress — this very call site.
        const auto sPos = projectToImageRect(camera, pos, imgMin, imgMax);
        const auto axisX = projectAxis(camera, pos,
            math::Vector<float, 3>{pos.v[0] + kAxisLengthWorld, pos.v[1], pos.v[2]},
            sPos, imgMin, imgMax);
        const auto axisY = projectAxis(camera, pos,
            math::Vector<float, 3>{pos.v[0], pos.v[1] + kAxisLengthWorld, pos.v[2]},
            sPos, imgMin, imgMax);
        const auto axisZ = projectAxis(camera, pos,
            math::Vector<float, 3>{pos.v[0], pos.v[1], pos.v[2] + kAxisLengthWorld},
            sPos, imgMin, imgMax);

        const ImVec2 mouse = ImGui::GetMousePos();
        const math::Vector<float, 2> mousePos{mouse.x, mouse.y};
        const bool mouseInImg =
            mouse.x >= imgMin.v[0] && mouse.x <= imgMax.v[0] &&
            mouse.y >= imgMin.v[1] && mouse.y <= imgMax.v[1];

        // Hit test. Skipped while a drag is active so the active axis
        // stays stable even if the cursor strays off the line.
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

        // Drag state machine.
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
            // Continue drag while the button is held; release ends it.
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
                        axisWorld = math::Vector<float, 3>{1.0F, 0.0F, 0.0F};
                        axisScreen = math::Vector<float, 2>{
                            axisX.end.v[0] - sPos.v[0],
                            axisX.end.v[1] - sPos.v[1]};
                        break;
                    case Axis::Y:
                        axisWorld = math::Vector<float, 3>{0.0F, 1.0F, 0.0F};
                        axisScreen = math::Vector<float, 2>{
                            axisY.end.v[0] - sPos.v[0],
                            axisY.end.v[1] - sPos.v[1]};
                        break;
                    case Axis::Z:
                        axisWorld = math::Vector<float, 3>{0.0F, 0.0F, 1.0F};
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
                    // Mouse delta from drag-start projected onto the
                    // axis screen vector. Dividing by length-squared
                    // and scaling by kAxisLengthWorld converts it from
                    // a pixel-space scalar to a world-space delta.
                    const float mdx = mousePos.v[0] - dragStartMouse_.v[0];
                    const float mdy = mousePos.v[1] - dragStartMouse_.v[1];
                    const float dot =
                        mdx * axisScreen.v[0] + mdy * axisScreen.v[1];
                    const float t = dot / screenLen2 * kAxisLengthWorld;
                    selectedActor->setPosition(math::Vector<float, 3>{
                        dragStartPosition_.v[0] + axisWorld.v[0] * t,
                        dragStartPosition_.v[1] + axisWorld.v[1] * t,
                        dragStartPosition_.v[2] + axisWorld.v[2] * t
                    });
                }
                claimedMouse = true;
            }
        }

        // Drawing. Highlight the hovered or active axis in the same
        // yellow used by the AABB selection so the user can read the
        // gizmo state without learning a new colour key.
        auto* drawList = ImGui::GetWindowDrawList();
        if (drawList)
        {
            drawList->PushClipRect(
                ImVec2{imgMin.v[0], imgMin.v[1]},
                ImVec2{imgMax.v[0], imgMax.v[1]},
                /*intersect_with_current_clip_rect*/ true);

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

            // Centre dot — a clear "this is the actor's pivot" marker
            // and gives the user something to aim near when the camera
            // collapses some axes.
            drawList->AddCircleFilled(origin, 4.0F,
                                      IM_COL32(255, 255, 255, 220));

            drawList->PopClipRect();
        }

        return claimedMouse;
    }
}
