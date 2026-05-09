#ifndef FLUXION_EDITOR_COMMANDS_ASSETCOMMANDS_HPP
#define FLUXION_EDITOR_COMMANDS_ASSETCOMMANDS_HPP

#include <string>
#include "Command.hpp"
#include "../../engine/assets/Asset.hpp"
#include "../../engine/utils/Uuid.hpp"

namespace fluxion::scene { class Layer; }

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
}

#endif // FLUXION_EDITOR_COMMANDS_ASSETCOMMANDS_HPP
