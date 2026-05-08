#include "InspectorPanel.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>
#include "imgui.h"
#include "../SelectionContext.hpp"
#include "../../engine/gui/imgui/ImGuiInspector.hpp"
#include "../../engine/math/Constants.hpp"
#include "../../engine/math/Quaternion.hpp"
#include "../../engine/reflection/TypeRegistry.hpp"
#include "../../engine/scene/Actor.hpp"
#include "../../engine/scene/Component.hpp"

namespace fluxion::editor
{
    namespace
    {
        constexpr std::size_t kNameBufferSize = 128;

        constexpr float radToDeg(float r) noexcept
        {
            return r * (180.0F / math::pi<float>);
        }

        constexpr float degToRad(float d) noexcept
        {
            return d * (math::pi<float> / 180.0F);
        }

        bool sameVec3(const math::Vector<float, 3>& a, const math::Vector<float, 3>& b)
        {
            return a.v[0] == b.v[0] && a.v[1] == b.v[1] && a.v[2] == b.v[2];
        }
    }

    InspectorPanel::InspectorPanel(SelectionContext* selection):
        Panel{"Inspector", true},
        selection_{selection}
    {
    }

    void InspectorPanel::draw()
    {
        ImGui::SetNextWindowPos(ImVec2(900, 200), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(360, 500), ImGuiCond_FirstUseEver);
        ImGui::Begin(displayName_.c_str());

        scene::Actor* actor = selection_ ? selection_->getSelected() : nullptr;
        if (!actor)
        {
            ImGui::TextDisabled("(no selection)");
            ImGui::TextDisabled("Click an actor in the Scene panel.");
            ImGui::End();
            return;
        }

        drawActorHeader(*actor);
        ImGui::Separator();
        drawTransform(*actor);
        ImGui::Separator();
        drawDisplayFlags(*actor);
        ImGui::Separator();
        drawComponents(*actor);

        ImGui::End();
    }

    void InspectorPanel::drawActorHeader(scene::Actor& actor)
    {
        // Identity is read-only — UUIDs are stable for the actor's
        // lifetime by design and the editor exposes no flow for
        // re-issuing one from the inspector.
        const auto idStr = actor.getId().toString();
        ImGui::TextDisabled("uuid: %s", idStr.c_str());

        char nameBuf[kNameBufferSize];
        const auto& current = actor.getName();
        // strncpy is deprecated under SDLCheck on MSVC; std::memcpy is
        // portable and explicit about the truncation we want.
        const auto copyLen = std::min(current.size(), kNameBufferSize - 1);
        std::memcpy(nameBuf, current.data(), copyLen);
        nameBuf[copyLen] = '\0';
        if (ImGui::InputText("Name", nameBuf, kNameBufferSize))
        {
            // Only call setName when the buffer actually differs to
            // avoid an allocation per frame on a stable name.
            if (std::strcmp(nameBuf, current.c_str()) != 0)
                actor.setName(std::string{nameBuf});
        }
    }

    void InspectorPanel::drawTransform(scene::Actor& actor)
    {
        if (!ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            return;

        // Position
        {
            auto pos = actor.getPosition();
            if (ImGui::DragFloat3("Position", pos.v, 0.1F))
                if (!sameVec3(pos, actor.getPosition()))
                    actor.setPosition(pos);
        }

        // Rotation: quaternion is internal; we expose Euler angles in
        // degrees because raw quat editing has unintuitive UX. Reading
        // back the quaternion as Euler each frame is cheap and matches
        // exactly what setRotation(Vector3) does on the way back in.
        {
            const auto eulerRad = math::getEulerAngles(actor.getRotation());
            float eulerDeg[3] = {
                radToDeg(eulerRad.v[0]),
                radToDeg(eulerRad.v[1]),
                radToDeg(eulerRad.v[2]),
            };
            if (ImGui::DragFloat3("Rotation (deg)", eulerDeg, 0.5F))
            {
                actor.setRotation(math::Vector<float, 3>{
                    degToRad(eulerDeg[0]),
                    degToRad(eulerDeg[1]),
                    degToRad(eulerDeg[2])
                });
            }
        }

        // Scale
        {
            auto scale = actor.getScale();
            if (ImGui::DragFloat3("Scale", scale.v, 0.01F))
                if (!sameVec3(scale, actor.getScale()))
                    actor.setScale(scale);
        }
    }

    void InspectorPanel::drawDisplayFlags(scene::Actor& actor)
    {
        if (!ImGui::CollapsingHeader("Display"))
            return;

        {
            float opacity = actor.getOpacity();
            if (ImGui::SliderFloat("Opacity", &opacity, 0.0F, 1.0F))
                if (opacity != actor.getOpacity())
                    actor.setOpacity(opacity);
        }
        {
            bool hidden = actor.isHidden();
            if (ImGui::Checkbox("Hidden", &hidden))
                if (hidden != actor.isHidden())
                    actor.setHidden(hidden);
        }
        {
            bool pickable = actor.isPickable();
            if (ImGui::Checkbox("Pickable", &pickable))
                if (pickable != actor.isPickable())
                    actor.setPickable(pickable);
        }
    }

    void InspectorPanel::drawComponents(scene::Actor& actor)
    {
        const auto& components = actor.getComponents();

        // Defer all add/remove ops until after the iteration finishes
        // so we never mutate the components vector mid-loop. Each is
        // applied below before drawComponents returns.
        scene::Component* removeTarget = nullptr;
        std::string addTypeName;

        if (components.empty())
        {
            ImGui::TextDisabled("(no components)");
        }
        else
        {
            gui::imgui::Inspector inspector;
            for (std::size_t i = 0; i < components.size(); ++i)
            {
                auto* comp = components[i];
                if (!comp) continue;

                const auto type = comp->typeName();
                const std::string label = !type.empty()
                    ? std::string{type}
                    : std::string{"<unregistered component>"};

                ImGui::PushID(static_cast<int>(i));
                char header[160];
                std::snprintf(header, sizeof(header), "%s##%zu",
                              label.c_str(), i);

                const bool open = ImGui::CollapsingHeader(
                    header, ImGuiTreeNodeFlags_DefaultOpen);

                // Per-component remove on the same row as the header,
                // visually separated to the right edge of the panel.
                ImGui::SameLine(ImGui::GetContentRegionAvail().x +
                                ImGui::GetCursorPosX() - 24.0F);
                if (ImGui::SmallButton("X"))
                    removeTarget = comp;
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Remove component");

                if (open)
                {
                    ImGui::Indent();
                    comp->inspect(inspector);
                    ImGui::Unindent();
                }
                ImGui::PopID();
            }
        }

        // Add Component button + popup. The popup lists every type
        // currently registered with the engine's TypeRegistry, so
        // anything someone adds via FLUXION_COMPONENT_REGISTER shows
        // up here automatically without editor-side wiring.
        ImGui::Separator();
        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("##add_component_popup");
        if (ImGui::BeginPopup("##add_component_popup"))
        {
            const auto types = reflection::TypeRegistry::instance().listTypes();
            if (types.empty())
                ImGui::TextDisabled("(no registered component types)");
            else
                for (const auto& t : types)
                    if (ImGui::MenuItem(t.c_str()))
                        addTypeName = t;
            ImGui::EndPopup();
        }

        // Apply deferred ops. Order matters only insofar as both can
        // happen in the same frame from independent UI paths — the
        // engine's addComponent/removeComponent are independent calls
        // and either order is safe.
        if (removeTarget)
            actor.removeComponent(*removeTarget);
        if (!addTypeName.empty())
        {
            auto comp = reflection::TypeRegistry::instance().create(addTypeName);
            if (comp) actor.addComponent(std::move(comp));
        }
    }
}
