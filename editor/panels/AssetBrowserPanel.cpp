#include "AssetBrowserPanel.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <optional>
#include <vector>
#include "imgui.h"
#include "../SelectionContext.hpp"
#include "../asset/AssetDatabase.hpp"
#include "../../engine/assets/Cache.hpp"
#include "../../engine/graphics/Texture.hpp"
#include "../../engine/gui/imgui/ImGuiContext.hpp"
#include "SpriteEditorPanel.hpp"

namespace fluxion::editor
{
    namespace
    {
        constexpr const char* kAssetDragDropId = "FLUXION_ASSET_REF";
        constexpr float kTileSize = 64.0F;
        constexpr float kTilePadding = 8.0F;

        // Returns "" for files at the project root, or the path's
        // parent directory in generic-form (forward slashes, no
        // trailing slash) for nested files.
        std::string parentFolder(const std::string& relPath)
        {
            const auto pos = relPath.find_last_of('/');
            return (pos == std::string::npos) ? std::string{} : relPath.substr(0, pos);
        }

        // For a folder `prefix` and a candidate folder path `path`,
        // return the immediate child segment if `path` is a direct
        // descendant of `prefix`, or std::nullopt otherwise. Used to
        // build the recursive folder-tree without sorting branches.
        std::optional<std::string> directChild(const std::string& prefix,
                                                const std::string& path)
        {
            if (prefix.empty())
            {
                // Root: a direct child has no '/' in its name.
                if (path.empty()) return std::nullopt;
                if (path.find('/') == std::string::npos) return path;
                return std::nullopt;
            }
            if (path.size() <= prefix.size() + 1) return std::nullopt;
            if (path.compare(0, prefix.size(), prefix) != 0) return std::nullopt;
            if (path[prefix.size()] != '/') return std::nullopt;
            const auto remainder = path.substr(prefix.size() + 1);
            if (remainder.find('/') == std::string::npos) return remainder;
            return std::nullopt;
        }

        // Single-letter glyph used for non-image asset tiles. Keeps
        // the panel readable when we don't have a thumbnail yet.
        const char* typeGlyph(assets::Asset::Type t) noexcept
        {
            using T = assets::Asset::Type;
            switch (t)
            {
                case T::image: return "I";
                case T::sprite: return "S";
                case T::sound: return "A";
                case T::cue: return "C";
                case T::font: return "T";
                case T::staticMesh: return "M";
                case T::skinnedMesh: return "M";
                case T::material: return "m";
                case T::particleSystem: return "P";
                case T::bank: return "B";
            }
            return "?";
        }

        ImU32 typeTileColour(assets::Asset::Type t) noexcept
        {
            using T = assets::Asset::Type;
            switch (t)
            {
                case T::image: return IM_COL32(80, 90, 110, 255);
                case T::sprite: return IM_COL32(70, 110, 130, 255);
                case T::sound: return IM_COL32(110, 80, 60, 255);
                case T::cue: return IM_COL32(120, 90, 70, 255);
                case T::font: return IM_COL32(90, 100, 80, 255);
                case T::staticMesh: return IM_COL32(80, 80, 100, 255);
                case T::skinnedMesh: return IM_COL32(95, 75, 110, 255);
                case T::material: return IM_COL32(70, 90, 80, 255);
                case T::particleSystem: return IM_COL32(110, 70, 80, 255);
                case T::bank: return IM_COL32(85, 85, 85, 255);
            }
            return IM_COL32(60, 60, 60, 255);
        }
    }

    AssetBrowserPanel::AssetBrowserPanel(AssetDatabase* database,
                                          assets::Cache* cache,
                                          gui::imgui::Context* imgui,
                                          SelectionContext* selection,
                                          SpriteEditorPanel* spriteEditor) noexcept:
        Panel{"Asset Browser", true},
        database_{database},
        cache_{cache},
        imgui_{imgui},
        selection_{selection},
        spriteEditor_{spriteEditor}
    {
    }

    void AssetBrowserPanel::collectFolders()
    {
        folderSet_.clear();
        if (!database_) return;
        for (const auto& entry : database_->entries())
        {
            std::string folder = parentFolder(entry.relativePath);
            // Insert this folder and every ancestor up to the root,
            // so the tree has the intermediate nodes even if no asset
            // sits directly in them.
            while (!folder.empty())
            {
                folderSet_.insert(folder);
                folder = parentFolder(folder);
            }
        }
    }

    void AssetBrowserPanel::drawFolderNode(const std::string& prefix)
    {
        // Find the immediate-child folders of `prefix`.
        std::vector<std::string> children;
        for (const auto& f : folderSet_)
            if (auto child = directChild(prefix, f))
                children.push_back(*child);
        std::sort(children.begin(), children.end());

        for (const auto& child : children)
        {
            const std::string fullPath = prefix.empty()
                ? child : (prefix + "/" + child);

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
                | ImGuiTreeNodeFlags_OpenOnDoubleClick
                | ImGuiTreeNodeFlags_SpanAvailWidth;
            if (selectedFolder_ == fullPath)
                flags |= ImGuiTreeNodeFlags_Selected;

            const bool open = ImGui::TreeNodeEx(child.c_str(), flags);
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                selectedFolder_ = fullPath;
            if (open)
            {
                drawFolderNode(fullPath);
                ImGui::TreePop();
            }
        }
    }

    void AssetBrowserPanel::draw()
    {
        ImGui::SetNextWindowPos(ImVec2(20, 600), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(700, 320), ImGuiCond_FirstUseEver);
        ImGui::Begin(displayName_.c_str());

        if (!database_ || !database_->isOpen())
        {
            ImGui::TextDisabled("(no project root)");
            ImGui::TextDisabled("Phase 11b initialises the database from "
                                "samples/Resources at startup.");
            ImGui::End();
            return;
        }

        collectFolders();

        const float treeWidth = 180.0F;

        // Left pane: folder tree.
        ImGui::BeginChild("##asset_tree", ImVec2(treeWidth, 0), true);
        {
            ImGuiTreeNodeFlags rootFlags = ImGuiTreeNodeFlags_OpenOnArrow
                | ImGuiTreeNodeFlags_OpenOnDoubleClick
                | ImGuiTreeNodeFlags_DefaultOpen
                | ImGuiTreeNodeFlags_SpanAvailWidth;
            if (selectedFolder_.empty())
                rootFlags |= ImGuiTreeNodeFlags_Selected;
            const bool rootOpen = ImGui::TreeNodeEx("(root)", rootFlags);
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                selectedFolder_.clear();
            if (rootOpen)
            {
                drawFolderNode("");
                ImGui::TreePop();
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // Right pane: tile grid for the selected folder.
        ImGui::BeginChild("##asset_grid", ImVec2(0, 0), true);
        {
            ImGui::TextDisabled("Folder: %s",
                                selectedFolder_.empty() ? "(root)"
                                                         : selectedFolder_.c_str());
            ImGui::Separator();

            const float availWidth = ImGui::GetContentRegionAvail().x;
            const float stride = kTileSize + kTilePadding;
            const int perRow = std::max(1, static_cast<int>(availWidth / stride));
            int columnIndex = 0;

            std::size_t count = 0;
            for (const auto& entry : database_->entries())
            {
                // Direct-children-of-selectedFolder filter.
                if (parentFolder(entry.relativePath) != selectedFolder_)
                    continue;

                if (columnIndex > 0) ImGui::SameLine();
                ImGui::PushID(static_cast<int>(count++));

                const ImVec2 cursor = ImGui::GetCursorScreenPos();
                const ImVec2 tileSize{kTileSize, kTileSize};
                const ImVec2 tileEnd{cursor.x + kTileSize, cursor.y + kTileSize};

                // Decoration drawn first via the window's draw list:
                // backdrop, then the image or fallback glyph. The
                // interactive InvisibleButton is stacked on top so it
                // owns click + drag-source semantics regardless of
                // whether we managed to render a thumbnail.
                ImDrawList* dl = ImGui::GetWindowDrawList();
                dl->AddRectFilled(cursor, tileEnd,
                                  typeTileColour(entry.type), 4.0F);

                bool drewThumbnail = false;
                if (entry.type == assets::Asset::Type::image && cache_ && imgui_)
                {
                    if (const auto tex = cache_->getTexture(entry.id))
                    {
                        const auto handle = imgui_->textureNativeHandle(*tex);
                        if (handle)
                        {
                            // ImTextureID is ImU64 in this ImGui build
                            // (typedef ImU64 ImTextureID in imgui.h).
                            dl->AddImage(static_cast<ImTextureID>(handle),
                                         cursor, tileEnd);
                            drewThumbnail = true;
                        }
                    }
                }
                else if (!entry.parentId.isNull() && cache_ && imgui_)
                {
                    // Sub-slice tile: sample a UV sub-rect of the
                    // parent's texture so the thumbnail shows just
                    // this slice, not the whole sheet. AssetEntry
                    // already carries the slice rect in pixels;
                    // convert to UV using the parent texture size.
                    if (const auto parentTex = cache_->getTexture(entry.parentId))
                    {
                        const auto handle = imgui_->textureNativeHandle(*parentTex);
                        if (handle)
                        {
                            const auto sz = parentTex->getSize();
                            const float texW = static_cast<float>(sz.v[0]);
                            const float texH = static_cast<float>(sz.v[1]);
                            if (texW > 0.0F && texH > 0.0F)
                            {
                                const ImVec2 uv0{
                                    entry.sliceRectPx.position.v[0] / texW,
                                    entry.sliceRectPx.position.v[1] / texH
                                };
                                const ImVec2 uv1{
                                    (entry.sliceRectPx.position.v[0] +
                                     entry.sliceRectPx.size.v[0]) / texW,
                                    (entry.sliceRectPx.position.v[1] +
                                     entry.sliceRectPx.size.v[1]) / texH
                                };
                                dl->AddImage(static_cast<ImTextureID>(handle),
                                             cursor, tileEnd, uv0, uv1);
                                drewThumbnail = true;
                            }
                        }
                    }
                }
                if (!drewThumbnail)
                {
                    const ImVec2 textSize = ImGui::CalcTextSize(typeGlyph(entry.type));
                    dl->AddText(ImVec2(cursor.x + (kTileSize - textSize.x) * 0.5F,
                                       cursor.y + (kTileSize - textSize.y) * 0.5F),
                                IM_COL32_WHITE, typeGlyph(entry.type));
                }

                // Selection highlight: a 2px outline drawn over the
                // decoration but under the InvisibleButton (which is
                // invisible by definition). Uses the SelectionContext's
                // asset channel so clicking a tree-row in the Scene
                // panel doesn't leave a stale highlight here.
                const bool isSelected = selection_ &&
                                        !selection_->getSelectedAssetId().isNull() &&
                                        selection_->getSelectedAssetId() == entry.id;
                if (isSelected)
                    dl->AddRect(cursor, tileEnd,
                                IM_COL32(255, 200, 60, 255),
                                4.0F, 0, 2.5F);

                // Stack the click target. ImGui::InvisibleButton
                // becomes the "current item" so BeginDragDropSource
                // and the release test attach to this rect, regardless
                // of which decoration we drew above.
                ImGui::SetCursorScreenPos(cursor);
                ImGui::InvisibleButton("##tile", tileSize);

                // Drag source: emit the AssetReference payload.
                // Submitted before the click test so ImGui has a
                // chance to flag this item as a drag source — when
                // the user drags, IsItemHovered() returns false on
                // the source (ImGui's default behaviour without
                // SourceNoDisableHover), so the click-on-release
                // path below correctly skips firing a selection.
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    assets::AssetReference ref{entry.id, entry.type};
                    ImGui::SetDragDropPayload(kAssetDragDropId,
                                              &ref, sizeof(ref));
                    // Tooltip during drag — handy for asset names.
                    ImGui::TextUnformatted(entry.name.c_str());
                    ImGui::EndDragDropSource();
                }

                // Select on release-while-hovered. This deliberately
                // does NOT fire on press: a press that begins a drag
                // would otherwise immediately switch the Inspector to
                // the asset-meta view and tear down the drop receptor
                // before the drag even starts moving. The hover check
                // also handles drag-then-release-elsewhere — ImGui
                // disables IsItemHovered on the source while a drag
                // is active, so dropping on a different target leaves
                // the source's selection state untouched.
                if (ImGui::IsItemHovered() &&
                    ImGui::IsMouseReleased(ImGuiMouseButton_Left) &&
                    selection_)
                    selection_->setSelectedAsset(entry.id);

                // Right-click context menu. Single-image parents
                // get "Open in Sprite Editor"; sub-slices and
                // non-image entries skip the menu altogether. The
                // popup id is per-tile (we're inside a PushID).
                if (entry.type == assets::Asset::Type::image &&
                    entry.parentId.isNull() &&
                    spriteEditor_ &&
                    ImGui::BeginPopupContextItem("##tile_ctx"))
                {
                    if (ImGui::MenuItem("Open in Sprite Editor"))
                        spriteEditor_->setTarget(entry.id);
                    ImGui::EndPopup();
                }

                // Caption underneath the tile, clipped to the tile
                // width so long names don't overflow into the next
                // column.
                ImGui::SetCursorScreenPos(ImVec2(cursor.x,
                                                  cursor.y + kTileSize + 2.0F));
                ImGui::PushTextWrapPos(cursor.x + kTileSize);
                ImGui::TextUnformatted(entry.name.c_str());
                ImGui::PopTextWrapPos();

                // Advance to next tile slot. ImGui's cursor is at the
                // bottom of our caption now; reset to the row's top
                // for SameLine to position the next tile correctly.
                ImGui::SetCursorScreenPos(ImVec2(cursor.x + kTileSize + kTilePadding,
                                                  cursor.y));
                ImGui::Dummy(ImVec2(0.0F, kTileSize + 18.0F));

                ImGui::PopID();

                ++columnIndex;
                if (columnIndex >= perRow)
                {
                    columnIndex = 0;
                    // Force the next tile to start a fresh row.
                    ImGui::NewLine();
                }
            }

            if (count == 0)
                ImGui::TextDisabled("(empty folder)");
        }
        ImGui::EndChild();

        ImGui::End();
    }
}
