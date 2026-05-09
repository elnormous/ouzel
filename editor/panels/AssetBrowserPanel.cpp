#include "AssetBrowserPanel.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <optional>
#include <vector>
#include "imgui.h"
#include "../asset/AssetDatabase.hpp"
#include "../../engine/assets/Cache.hpp"
#include "../../engine/graphics/Texture.hpp"
#include "../../engine/gui/imgui/ImGuiContext.hpp"

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
                                          gui::imgui::Context* imgui) noexcept:
        Panel{"Asset Browser", true},
        database_{database},
        cache_{cache},
        imgui_{imgui}
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

                // Backdrop tile so audio/font/etc. files have a
                // visible click target the same size as the texture
                // thumbnails.
                ImDrawList* dl = ImGui::GetWindowDrawList();
                dl->AddRectFilled(cursor,
                                  ImVec2(cursor.x + kTileSize,
                                         cursor.y + kTileSize),
                                  typeTileColour(entry.type),
                                  4.0F);

                bool drewThumbnail = false;
                if (entry.type == assets::Asset::Type::image && cache_ && imgui_)
                {
                    if (const auto tex = cache_->getTexture(entry.id))
                    {
                        const auto handle = imgui_->textureNativeHandle(*tex);
                        if (handle)
                        {
                            ImGui::SetCursorScreenPos(cursor);
                            // ImTextureID is ImU64 in this ImGui build
                            // (see external/imgui/imgui.h: typedef ImU64
                            // ImTextureID), same as in ViewportPanel.
                            ImGui::Image(static_cast<ImTextureID>(handle),
                                         ImVec2(kTileSize, kTileSize));
                            drewThumbnail = true;
                        }
                    }
                }
                if (!drewThumbnail)
                {
                    // Draw the type glyph centred on the backdrop. We
                    // still need an ImGui item that occupies the same
                    // rect so drag-drop and click detection work.
                    ImGui::SetCursorScreenPos(cursor);
                    ImGui::InvisibleButton("##tile",
                                           ImVec2(kTileSize, kTileSize));
                    const ImVec2 textSize = ImGui::CalcTextSize(typeGlyph(entry.type));
                    dl->AddText(ImVec2(cursor.x + (kTileSize - textSize.x) * 0.5F,
                                       cursor.y + (kTileSize - textSize.y) * 0.5F),
                                IM_COL32_WHITE, typeGlyph(entry.type));
                }

                // Drag source: emit the AssetReference payload.
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    assets::AssetReference ref{entry.id, entry.type};
                    ImGui::SetDragDropPayload(kAssetDragDropId,
                                              &ref, sizeof(ref));
                    // Tooltip during drag — handy for asset names.
                    ImGui::TextUnformatted(entry.name.c_str());
                    ImGui::EndDragDropSource();
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
