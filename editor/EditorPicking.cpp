#include "EditorPicking.hpp"

#include <algorithm>
#include <array>
#include <limits>
#include "imgui.h"
#include "../engine/math/Box.hpp"
#include "../engine/math/Matrix.hpp"
#include "../engine/scene/Actor.hpp"
#include "../engine/scene/Camera.hpp"
#include "../engine/scene/Layer.hpp"
#include "../engine/scene/Scene.hpp"

namespace fluxion::editor
{
    namespace
    {
        // The fallback editor AABB for actors that don't bring their
        // own bounding box. Half a unit each side around the local
        // origin gives a 1x1x1 click target in local space, which is
        // visually distinct without dominating the view.
        constexpr float kFallbackBoxHalf = 0.5F;

        // 8 corners of an axis-aligned 3D box, in canonical order.
        std::array<math::Vector<float, 3>, 8> boxCorners(const math::Box<float, 3>& b)
        {
            return {
                math::Vector<float, 3>{b.min.v[0], b.min.v[1], b.min.v[2]},
                math::Vector<float, 3>{b.max.v[0], b.min.v[1], b.min.v[2]},
                math::Vector<float, 3>{b.max.v[0], b.max.v[1], b.min.v[2]},
                math::Vector<float, 3>{b.min.v[0], b.max.v[1], b.min.v[2]},
                math::Vector<float, 3>{b.min.v[0], b.min.v[1], b.max.v[2]},
                math::Vector<float, 3>{b.max.v[0], b.min.v[1], b.max.v[2]},
                math::Vector<float, 3>{b.max.v[0], b.max.v[1], b.max.v[2]},
                math::Vector<float, 3>{b.min.v[0], b.max.v[1], b.max.v[2]},
            };
        }

        // The 12 edges of a box, indexed into the boxCorners() array.
        constexpr std::array<std::pair<int, int>, 12> kBoxEdges = {{
            {0, 1}, {1, 2}, {2, 3}, {3, 0},  // bottom face (z = min)
            {4, 5}, {5, 6}, {6, 7}, {7, 4},  // top face (z = max)
            {0, 4}, {1, 5}, {2, 6}, {3, 7},  // vertical edges
        }};

        math::Box<float, 3> editorBox(const scene::Actor& a)
        {
            auto box = a.getBoundingBox();
            if (math::isEmpty(box))
            {
                box.min = math::Vector<float, 3>{
                    -kFallbackBoxHalf, -kFallbackBoxHalf, -kFallbackBoxHalf};
                box.max = math::Vector<float, 3>{
                    kFallbackBoxHalf, kFallbackBoxHalf, kFallbackBoxHalf};
            }
            return box;
        }

        // Project all 8 corners of `localBox` (after applying
        // `worldTransform`) through the camera and return the resulting
        // 8 normalized window-space positions. The caller picks what to
        // do with them (axis-aligned bounds for picking; line drawing
        // for the overlay).
        std::array<math::Vector<float, 2>, 8> projectCorners(
            const math::Box<float, 3>& localBox,
            const math::Matrix<float, 4>& worldTransform,
            const scene::Camera& camera)
        {
            const auto local = boxCorners(localBox);
            std::array<math::Vector<float, 2>, 8> result;
            for (std::size_t i = 0; i < 8; ++i)
            {
                auto worldP = local[i];
                math::transformPoint(worldTransform, worldP);
                result[i] = camera.convertWorldToNormalized(worldP);
            }
            return result;
        }

        // 2D axis-aligned hit test: does `click` fall inside the
        // bounding rectangle of `corners` (in normalized [0,1] space)?
        bool hitProjectedBox(const std::array<math::Vector<float, 2>, 8>& corners,
                             const math::Vector<float, 2>& click)
        {
            float minX = std::numeric_limits<float>::max();
            float minY = std::numeric_limits<float>::max();
            float maxX = std::numeric_limits<float>::lowest();
            float maxY = std::numeric_limits<float>::lowest();
            for (const auto& c : corners)
            {
                minX = std::min(minX, c.v[0]);
                minY = std::min(minY, c.v[1]);
                maxX = std::max(maxX, c.v[0]);
                maxY = std::max(maxY, c.v[1]);
            }
            return click.v[0] >= minX && click.v[0] <= maxX
                && click.v[1] >= minY && click.v[1] <= maxY;
        }

        template <typename Visitor>
        void walkActors(scene::ActorContainer& container, Visitor&& visit)
        {
            for (auto* child : container.getChildren())
            {
                if (!child) continue;
                visit(*child);
                walkActors(*child, visit);
            }
        }
    }

    scene::Actor* pickActor(scene::Scene& scene,
                            scene::Camera& camera,
                            const math::Vector<float, 2>& normalizedPos,
                            const scene::Layer* excludeLayer,
                            const scene::Actor* excludeActor)
    {
        scene::Actor* topmost = nullptr;
        // Iterate front-to-back is hard in a tree without depth info;
        // we settle for "last hit wins". The visit order is parent
        // before child, so children (drawn on top of parents in the
        // engine's render order) take precedence as desired.
        for (auto* layer : scene.getLayers())
        {
            if (!layer || layer == excludeLayer) continue;
            walkActors(*layer, [&](scene::Actor& actor) {
                if (actor.isHidden() || &actor == excludeActor) return;
                const auto box = editorBox(actor);
                const auto projected = projectCorners(
                    box, actor.getTransform(), camera);
                if (hitProjectedBox(projected, normalizedPos))
                    topmost = &actor;
            });
        }
        return topmost;
    }

    void drawAabbOverlay(scene::Scene& scene,
                         scene::Camera& camera,
                         scene::Actor* selectedActor,
                         const scene::Layer* excludeLayer,
                         const scene::Actor* excludeActor)
    {
        const auto displaySize = ImGui::GetIO().DisplaySize;
        if (displaySize.x <= 0.0F || displaySize.y <= 0.0F)
            return;
        const math::Vector<float, 2> windowSizePixels{
            displaySize.x, displaySize.y
        };

        // Background draw list renders BEHIND all ImGui windows, but
        // still in front of the engine's back buffer — exactly the
        // visual layer we want for actor decorations: panels stay
        // readable, the editor's overlay stays anchored to the world.
        auto* drawList = ImGui::GetBackgroundDrawList();
        if (!drawList) return;

        const ImU32 dimColor = IM_COL32(140, 140, 160, 110);
        const ImU32 selColor = IM_COL32(255, 180, 60, 230);

        for (auto* layer : scene.getLayers())
        {
            if (!layer || layer == excludeLayer) continue;
            walkActors(*layer, [&](scene::Actor& actor) {
                if (actor.isHidden() || &actor == excludeActor) return;
                const auto box = editorBox(actor);
                const auto projected = projectCorners(
                    box, actor.getTransform(), camera);

                const ImU32 color = (&actor == selectedActor) ? selColor : dimColor;
                const float thickness = (&actor == selectedActor) ? 2.5F : 1.0F;

                // Convert the 8 normalized corners to ImGui screen pixels
                // once, then draw the 12 box edges using indices.
                std::array<ImVec2, 8> screen{};
                float minX = std::numeric_limits<float>::max();
                float minY = std::numeric_limits<float>::max();
                for (std::size_t i = 0; i < 8; ++i)
                {
                    screen[i] = ImVec2{
                        projected[i].v[0] * windowSizePixels.v[0],
                        projected[i].v[1] * windowSizePixels.v[1]
                    };
                    minX = std::min(minX, screen[i].x);
                    minY = std::min(minY, screen[i].y);
                }
                for (const auto& edge : kBoxEdges)
                    drawList->AddLine(screen[edge.first], screen[edge.second],
                                      color, thickness);

                // Label at the AABB's top-left so the user can match
                // the overlay to the Scene tree. Falls back to a short
                // UUID prefix when the actor has no name.
                const auto label = !actor.getName().empty()
                    ? actor.getName()
                    : (std::string{"<"} + actor.getId().toString().substr(0, 8) + ">");
                drawList->AddText(ImVec2{minX + 2.0F, minY - 16.0F},
                                  color, label.c_str());
            });
        }
    }
}
