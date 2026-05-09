#ifndef FLUXION_EDITOR_COMMANDS_ASSETCOMMANDS_HPP
#define FLUXION_EDITOR_COMMANDS_ASSETCOMMANDS_HPP

#include <memory>
#include <string>
#include "Command.hpp"
#include "../../engine/assets/Asset.hpp"
#include "../../engine/math/Vector.hpp"
#include "../../engine/utils/Uuid.hpp"

namespace fluxion::scene { class Actor; class Layer; }

namespace fluxion::editor
{
    class SelectionContext;
}

namespace fluxion::editor::commands
{
    // Reverse a single asset-reference assignment on a component.
    //
    // Identifies the target by (actor uuid, component index, field label)
    // — all stable across undo/redo provided the user doesn't reorder
    // components manually (we don't expose reordering yet). A null layer
    // or missing actor means the command no-ops cleanly, matching the
    // policy of the other transform/component commands.
    class SetAssetCommand final : public Command
    {
    public:
        SetAssetCommand(scene::Layer* userLayer,
                        Uuid actorId,
                        std::size_t componentIndex,
                        std::string fieldLabel,
                        assets::AssetReference oldValue,
                        assets::AssetReference newValue) noexcept;

        void redo() override;
        void undo() override;
        [[nodiscard]] std::string label() const override;

    private:
        void applyValue(const assets::AssetReference& value) const;

        scene::Layer* userLayer_;     // not owned; nullable
        Uuid actorId_;
        std::size_t componentIndex_;
        std::string fieldLabel_;
        assets::AssetReference oldValue_;
        assets::AssetReference newValue_;
    };

    // Spawn a new top-level actor under `layer` carrying a single
    // SpriteRenderer pointed at `assetRef`. Used by the viewport's
    // drop-to-create flow: the user drags a tile onto the empty
    // viewport area, the panel converts the drop position to world
    // coords and pushes one of these. Single-step undo (delete +
    // detach) takes the actor back out of the layer.
    //
    // Ownership follows the AddActorCommand pattern: the command
    // alternately holds the unique_ptr (after ctor / after undo) or
    // hands it off to the parent container (after redo). The actor's
    // uuid is captured at construction so re-resolves after undo
    // round-trips work the same as AddActor.
    class CreateActorWithSpriteCommand final : public Command
    {
    public:
        CreateActorWithSpriteCommand(scene::Layer* layer,
                                     const assets::AssetReference& assetRef,
                                     const math::Vector<float, 3>& worldPosition,
                                     std::string actorName,
                                     SelectionContext* selection);
        ~CreateActorWithSpriteCommand() override;

        void redo() override;
        void undo() override;
        [[nodiscard]] std::string label() const override;

    private:
        scene::Layer* layer_;
        Uuid actorId_;
        std::unique_ptr<scene::Actor> stored_;
        SelectionContext* selection_;
    };
}

#endif // FLUXION_EDITOR_COMMANDS_ASSETCOMMANDS_HPP
