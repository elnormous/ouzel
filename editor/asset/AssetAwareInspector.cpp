#include "AssetAwareInspector.hpp"

#include <cstdio>
#include <cstring>
#include "imgui.h"
#include "AssetDatabase.hpp"

namespace fluxion::editor
{
    namespace
    {
        // Drag-drop payload identifier shared with AssetBrowserPanel.
        // ImGui requires <= 32 chars including the null terminator.
        constexpr const char* kAssetDragDropId = "FLUXION_ASSET_REF";
    }

    void AssetAwareInspector::assetField(const char* label,
                                          assets::AssetReference& value)
    {
        // Resolve the current asset's display name. When the database
        // is missing or the id doesn't resolve, we fall back to the
        // raw uuid string so the user still sees something stable
        // they can match against the .meta sidecars on disk.
        std::string resolved;
        if (value.id.isNull())
            resolved = "(none)";
        else if (database_)
        {
            if (const auto* entry = database_->findById(value.id))
                resolved = entry->relativePath;
            else
                resolved = "<missing> " + value.id.toString();
        }
        else
            resolved = value.id.toString();

        // Render the receptor as a wide button. Visually consistent
        // with ImGui's other inputs because we keep the text label
        // on the right via SameLine. Clicking does nothing on its
        // own — the future "..." picker hooks here.
        ImGui::PushID(label);
        char btn[160];
        std::snprintf(btn, sizeof(btn), "[%s] %s",
                      assets::typeName(value.type), resolved.c_str());
        const float fullWidth = ImGui::CalcItemWidth();
        ImGui::Button(btn, ImVec2(fullWidth, 0.0F));

        // Drop target. ImGui delivers the payload (an AssetReference
        // value) only when the mouse is released over this button.
        if (ImGui::BeginDragDropTarget())
        {
            if (const auto* payload =
                    ImGui::AcceptDragDropPayload(kAssetDragDropId))
            {
                if (payload->DataSize == static_cast<int>(sizeof(assets::AssetReference)))
                {
                    assets::AssetReference incoming;
                    std::memcpy(&incoming, payload->Data,
                                sizeof(assets::AssetReference));

                    // Mutate live so the user sees the change this
                    // frame, AND record the swap so the panel can
                    // turn it into an undoable command. Drag-drop
                    // can't fire twice in the same frame, so a
                    // single optional is enough.
                    if (incoming != value)
                    {
                        pending_ = PendingDrop{label, value, incoming};
                        value = incoming;
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::SameLine();
        ImGui::TextUnformatted(label);
        ImGui::PopID();
    }
}
