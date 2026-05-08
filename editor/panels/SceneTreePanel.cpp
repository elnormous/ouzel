#include "SceneTreePanel.hpp"

#include <cstdio>
#include "imgui.h"
#include "../SelectionContext.hpp"
#include "../../engine/scene/Actor.hpp"
#include "../../engine/scene/Component.hpp"
#include "../../engine/scene/Layer.hpp"
#include "../../engine/scene/Scene.hpp"

namespace fluxion::editor
{
    namespace
    {
        // Compose the label for one actor row. Falls back to a short
        // UUID prefix when the user hasn't named the actor, so anonymous
        // entries are still distinguishable in the tree.
        std::string actorLabel(const scene::Actor& actor)
        {
            if (!actor.getName().empty()) return actor.getName();
            const auto idStr = actor.getId().toString();
            return std::string{"<actor "} + idStr.substr(0, 8) + ">";
        }
    }

    SceneTreePanel::SceneTreePanel(scene::Scene* scene, SelectionContext* selection):
        Panel{"Scene", true},
        scene_{scene},
        selection_{selection}
    {
    }

    void SceneTreePanel::draw()
    {
        ImGui::SetNextWindowPos(ImVec2(20, 200), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(280, 360), ImGuiCond_FirstUseEver);
        ImGui::Begin(displayName_.c_str());

        if (!scene_)
        {
            ImGui::TextDisabled("(no scene)");
            ImGui::End();
            return;
        }

        // Drop dangling selection before rendering rows so the highlight
        // logic below never touches a freed pointer.
        if (selection_) selection_->validateAgainst(*scene_);

        const auto& layers = scene_->getLayers();
        if (layers.empty())
        {
            ImGui::TextDisabled("(scene has no layers)");
            ImGui::End();
            return;
        }

        for (std::size_t i = 0; i < layers.size(); ++i)
        {
            auto* layer = layers[i];
            if (!layer) continue;

            char header[64];
            std::snprintf(header, sizeof(header), "Layer %zu (order %d)##layer%zu",
                          i, layer->getOrder(), i);

            ImGui::PushID(layer);
            const bool open = ImGui::CollapsingHeader(header, ImGuiTreeNodeFlags_DefaultOpen);
            if (open) drawContainerChildren(*layer);
            ImGui::PopID();
        }

        ImGui::End();
    }

    void SceneTreePanel::drawContainerChildren(scene::ActorContainer& container)
    {
        const auto& kids = container.getChildren();
        for (auto* a : kids)
            if (a) drawActor(*a);
    }

    void SceneTreePanel::drawActor(scene::Actor& actor)
    {
        const auto label = actorLabel(actor);
        const bool isSelected = selection_ && selection_->getSelected() == &actor;

        // PushID per actor so siblings with identical names don't share
        // the same ImGui state (collapse, selection, etc.).
        ImGui::PushID(&actor);

        ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_OpenOnDoubleClick |
            ImGuiTreeNodeFlags_SpanAvailWidth;
        if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;

        const auto& children = actor.getChildren();
        const bool isLeaf = children.empty();
        if (isLeaf) flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        const bool open = ImGui::TreeNodeEx("##node", flags, "%s", label.c_str());

        // Component badges to the right of the label, e.g. "[2c]" if
        // the actor has components attached. Cheap visual cue without
        // dumping the full component list into the tree row.
        if (const auto compCount = actor.getComponents().size(); compCount > 0)
        {
            ImGui::SameLine();
            ImGui::TextDisabled("[%zuc]", compCount);
        }

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            if (selection_) selection_->setSelected(&actor);

        if (open && !isLeaf)
        {
            drawContainerChildren(actor);
            ImGui::TreePop();
        }

        ImGui::PopID();
    }
}
