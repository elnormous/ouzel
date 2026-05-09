#ifndef FLUXION_EDITOR_ASSETAWAREINSPECTOR_HPP
#define FLUXION_EDITOR_ASSETAWAREINSPECTOR_HPP

#include <optional>
#include <string>
#include "../../engine/assets/Asset.hpp"
#include "../../engine/gui/imgui/ImGuiInspector.hpp"

namespace fluxion::editor
{
    class AssetDatabase;

    // ImGuiInspector subclass that knows how to render an
    // AssetReference field as a draggable receptor: shows the
    // resolved asset's name (looked up via AssetDatabase) instead of
    // the raw uuid, and accepts the "FLUXION_ASSET_REF" ImGui
    // drag-drop payload from the AssetBrowserPanel.
    //
    // After comp->inspect(inspector) returns, the panel inspects
    // pendingDrop() to see whether a drop happened and route it
    // through CommandStack as a SetAssetCommand. Reset between
    // components with clearPendingDrop().
    class AssetAwareInspector final : public gui::imgui::Inspector
    {
    public:
        explicit AssetAwareInspector(const AssetDatabase* database) noexcept:
            database_{database} {}

        void assetField(const char* label, assets::AssetReference& value) override;

        struct PendingDrop final
        {
            std::string label;
            assets::AssetReference oldValue;
            assets::AssetReference newValue;
        };

        // Pop the pending drop event (if any). Returns std::nullopt
        // when no drop occurred since the last clear/pop. Designed to
        // be called once per `comp->inspect(...)` so the panel can
        // turn it into a SetAssetCommand.
        std::optional<PendingDrop> takePendingDrop() noexcept
        {
            auto result = std::move(pending_);
            pending_.reset();
            return result;
        }

        void clearPendingDrop() noexcept { pending_.reset(); }

    private:
        const AssetDatabase* database_;     // not owned, nullable
        std::optional<PendingDrop> pending_;
    };
}

#endif // FLUXION_EDITOR_ASSETAWAREINSPECTOR_HPP
