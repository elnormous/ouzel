#include "SceneTreePanel.hpp"

#include <cstdio>
#include <memory>
#include "imgui.h"
#include "../CommandStack.hpp"
#include "../SelectionContext.hpp"
#include "../commands/ActorCommands.hpp"
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

    SceneTreePanel::SceneTreePanel(scene::Scene* scene,
                                   SelectionContext* selection,
                                   CommandStack* commandStack,
                                   const scene::Layer* excludeLayer,
                                   const scene::Actor* excludeActor):
        Panel{"Scene", true},
        scene_{scene},
        selection_{selection},
        commandStack_{commandStack},
        excludeLayer_{excludeLayer},
        excludeActor_{excludeActor}
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
            if (!layer || layer == excludeLayer_) continue;

            char header[64];
            std::snprintf(header, sizeof(header), "Layer %zu (order %d)##layer%zu",
                          i, layer->getOrder(), i);

            ImGui::PushID(layer);
            const bool open = ImGui::CollapsingHeader(header, ImGuiTreeNodeFlags_DefaultOpen);
            // Right-click on the layer header opens an "Add Actor"
            // shortcut. Mutations are deferred so the tree iteration
            // below is unaffected.
            if (ImGui::BeginPopupContextItem("##layer_ctx"))
            {
                if (ImGui::MenuItem("Add Actor"))
                    pendingOps_.push_back(PendingOp{
                        PendingOp::Kind::AddChild, layer, nullptr, nullptr});
                ImGui::EndPopup();
            }
            if (open) drawContainerChildren(*layer);
            ImGui::PopID();
        }

        ImGui::End();

        // Mutations apply once the panel's draw walk is complete.
        // Triggers SelectionContext re-validation so the inspector
        // panel (drawn next in the same frame) doesn't inherit a
        // dangling pointer when the user just deleted an actor.
        applyPendingOps();
        if (selection_) selection_->validateAgainst(*scene_);
    }

    void SceneTreePanel::drawContainerChildren(scene::ActorContainer& container)
    {
        const auto& kids = container.getChildren();
        for (auto* a : kids)
        {
            if (!a) continue;
            if (a == excludeActor_) continue;
            // Editor-internal actors use a "<...>" naming convention
            // so the user can recognise (and so post-save/load round-
            // trips that lose pointer identity still hide them).
            // The viewport camera lives in workingLayer_ alongside
            // user content because the engine ties Camera component
            // membership to layer.children — but the user shouldn't
            // see it in their hierarchy.
            const auto& name = a->getName();
            if (!name.empty() && name.front() == '<') continue;
            drawActor(*a);
        }
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

        // Capture the click + toggle state for the TreeNode RIGHT
        // HERE, before any further widgets get submitted. Subsequent
        // ImGui calls (BeginPopupContextItem, the [Nc] badge below)
        // would shift the "last submitted item" away from the tree
        // node — IsItemClicked() at the end of the function would
        // then read the badge's state and miss real row clicks for
        // actors that own any component.
        const bool nodeClicked = ImGui::IsItemClicked();
        const bool nodeToggled = ImGui::IsItemToggledOpen();

        // Right-click context menu: spawn a child or delete this
        // actor. BeginPopupContextItem reads the last item too, but
        // we're still right after TreeNodeEx so it correctly anchors
        // to the row. Mutations are deferred — the queue is drained
        // after the whole tree walk is complete.
        if (ImGui::BeginPopupContextItem("##actor_ctx"))
        {
            if (ImGui::MenuItem("Add Child Actor"))
                pendingOps_.push_back(PendingOp{
                    PendingOp::Kind::AddChild, nullptr, &actor, nullptr});
            ImGui::Separator();
            if (ImGui::MenuItem("Delete"))
                pendingOps_.push_back(PendingOp{
                    PendingOp::Kind::Delete, nullptr, nullptr, &actor});
            ImGui::EndPopup();
        }

        // Component badge: shifts the "last item" pointer; we already
        // captured the row's click state above, so this is purely
        // decorative.
        if (const auto compCount = actor.getComponents().size(); compCount > 0)
        {
            ImGui::SameLine();
            ImGui::TextDisabled("[%zuc]", compCount);
        }

        if (nodeClicked && !nodeToggled)
            if (selection_) selection_->setSelected(&actor);

        if (open && !isLeaf)
        {
            drawContainerChildren(actor);
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    void SceneTreePanel::applyPendingOps()
    {
        // Routes deferred mutations through the command stack so the
        // user can step them back with Ctrl+Z. When no stack is
        // wired (legacy callers) we fall back to direct mutation —
        // unused in the editor today but keeps the panel reusable.
        for (const auto& op : pendingOps_)
        {
            switch (op.kind)
            {
                case PendingOp::Kind::AddChild:
                {
                    auto child = std::make_unique<scene::Actor>();
                    child->setName("Actor");
                    if (commandStack_)
                    {
                        if (op.parentLayer)
                            commandStack_->execute(
                                std::make_unique<commands::AddActorCommand>(
                                    op.parentLayer, std::move(child),
                                    selection_));
                        else if (op.parentActor && op.parentActor->getLayer())
                            commandStack_->execute(
                                std::make_unique<commands::AddActorCommand>(
                                    op.parentActor->getLayer(),
                                    op.parentActor->getId(),
                                    std::move(child),
                                    selection_));
                    }
                    else
                    {
                        if (op.parentLayer) op.parentLayer->addChild(std::move(child));
                        else if (op.parentActor) op.parentActor->addChild(std::move(child));
                    }
                    break;
                }
                case PendingOp::Kind::Delete:
                {
                    if (!op.deleteTarget) break;
                    if (commandStack_)
                    {
                        // Walk up to find the user layer (parents
                        // can be nested actors, but the topmost
                        // ancestor is always a layer).
                        auto* layer = op.deleteTarget->getLayer();
                        if (layer)
                            commandStack_->execute(
                                std::make_unique<commands::DeleteActorCommand>(
                                    layer, op.deleteTarget->getId(), selection_));
                    }
                    else
                    {
                        if (selection_ && selection_->getSelected() == op.deleteTarget)
                            selection_->clear();
                        if (auto* parent = op.deleteTarget->getParent())
                            parent->removeChild(*op.deleteTarget);
                    }
                    break;
                }
            }
        }
        pendingOps_.clear();
    }
}
