#include "SpriteEditorPanel.hpp"

#include <algorithm>
#include <cstdio>
#include "imgui.h"
#include "../asset/AssetImportPipeline.hpp"
#include "../../engine/assets/Cache.hpp"
#include "../../engine/graphics/Texture.hpp"
#include "../../engine/gui/imgui/ImGuiContext.hpp"

namespace fluxion::editor
{
    SpriteEditorPanel::SpriteEditorPanel(AssetDatabase* database,
                                          assets::Bundle* projectBundle,
                                          assets::Cache* cache,
                                          gui::imgui::Context* imgui) noexcept:
        Panel{"Sprite Editor", true},
        database_{database},
        projectBundle_{projectBundle},
        cache_{cache},
        imgui_{imgui}
    {
    }

    void SpriteEditorPanel::setTarget(const Uuid& parentId)
    {
        if (parentId.isNull())
        {
            targetId_ = Uuid{};
            workingSet_ = SpriteSliceSet{};
            columns_ = 1;
            rows_ = 1;
            return;
        }
        if (!database_) return;
        const auto* entry = database_->findById(parentId);
        if (!entry || !entry->parentId.isNull()) return;     // not a parent
        if (entry->type != assets::Asset::Type::image) return;

        targetId_ = parentId;
        workingSet_ = database_->getSliceSet(parentId);
        columns_ = std::max(1, workingSet_.columns);
        rows_    = std::max(1, workingSet_.rows);
    }

    void SpriteEditorPanel::applyGrid()
    {
        if (targetId_.isNull() || !cache_) return;
        const auto tex = cache_->getTexture(targetId_);
        if (!tex) return;

        const auto texSize = tex->getSize();
        const float texW = static_cast<float>(texSize.v[0]);
        const float texH = static_cast<float>(texSize.v[1]);
        const int cols = std::max(1, columns_);
        const int rws  = std::max(1, rows_);
        const float cellW = texW / static_cast<float>(cols);
        const float cellH = texH / static_cast<float>(rws);

        // Preserve existing slice ids by index. New cells get fresh
        // Uuids; trimmed cells (when shrinking the grid) are dropped.
        // Names follow a predictable "<stem>_<index>" pattern so the
        // user has something readable in the AssetBrowser straight
        // away — they can rename later in a future panel iteration.
        const auto* parent = database_->findById(targetId_);
        const std::string stem = parent ? parent->name : "slice";

        std::vector<SpriteSlice> next;
        next.reserve(static_cast<std::size_t>(cols * rws));
        for (int row = 0; row < rws; ++row)
        {
            for (int col = 0; col < cols; ++col)
            {
                const auto idx = static_cast<std::size_t>(row * cols + col);
                SpriteSlice slice;
                if (idx < workingSet_.slices.size())
                    slice.id = workingSet_.slices[idx].id;
                else
                    slice.id = Uuid::generate();
                slice.name = stem + "_" + std::to_string(idx);
                slice.rectPx = math::Rect<float>{
                    static_cast<float>(col) * cellW,
                    static_cast<float>(row) * cellH,
                    cellW, cellH
                };
                next.push_back(std::move(slice));
            }
        }

        workingSet_.mode = SpriteMode::sliced;
        workingSet_.columns = cols;
        workingSet_.rows = rws;
        workingSet_.slices = std::move(next);
    }

    bool SpriteEditorPanel::saveAndReimport()
    {
        if (targetId_.isNull() || !database_) return false;
        if (!database_->saveSliceSet(targetId_, workingSet_)) return false;
        if (projectBundle_)
            (void)importSlicesFor(*projectBundle_, *database_, targetId_);
        return true;
    }

    void SpriteEditorPanel::draw()
    {
        ImGui::SetNextWindowPos(ImVec2(720, 80), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(420, 420), ImGuiCond_FirstUseEver);
        ImGui::Begin(displayName_.c_str());

        if (targetId_.isNull())
        {
            ImGui::TextDisabled("(no asset)");
            ImGui::TextDisabled("Right-click an image in the Asset Browser");
            ImGui::TextDisabled("and pick 'Open in Sprite Editor'.");
            ImGui::End();
            return;
        }

        const auto* parent = database_ ? database_->findById(targetId_) : nullptr;
        if (!parent)
        {
            ImGui::TextDisabled("(asset no longer in database)");
            ImGui::End();
            return;
        }

        ImGui::Text("Asset: %s", parent->name.c_str());
        ImGui::Separator();

        // Mode selector. Single mode means "no slices" — explicitly
        // saving in this mode wipes the slice list.
        const char* modeNames[] = { "Single", "Sliced (auto-grid)" };
        int modeIndex = (workingSet_.mode == SpriteMode::sliced) ? 1 : 0;
        if (ImGui::Combo("Mode", &modeIndex, modeNames,
                         IM_ARRAYSIZE(modeNames)))
        {
            workingSet_.mode = (modeIndex == 1)
                ? SpriteMode::sliced
                : SpriteMode::single;
        }

        if (workingSet_.mode == SpriteMode::sliced)
        {
            // Grid inputs. DragInt instead of InputInt because it
            // gives a smoother UX for the small range we expect
            // (1-32 columns / rows is plenty for typical sheets).
            if (ImGui::DragInt("Columns", &columns_, 0.1F, 1, 64))
                columns_ = std::max(1, columns_);
            if (ImGui::DragInt("Rows", &rows_, 0.1F, 1, 64))
                rows_ = std::max(1, rows_);

            if (ImGui::Button("Apply Grid"))
                applyGrid();
            ImGui::SameLine();
        }

        if (ImGui::Button("Save"))
        {
            if (workingSet_.mode == SpriteMode::single)
                workingSet_.slices.clear();
            saveAndReimport();
        }
        ImGui::SameLine();
        if (ImGui::Button("Close"))
            setTarget(Uuid{});

        ImGui::Separator();

        // Texture preview with grid overlay. Box scales to fit the
        // panel width, preserving the texture's native aspect — same
        // pattern InspectorPanel uses for asset previews.
        if (cache_ && imgui_)
        {
            if (const auto tex = cache_->getTexture(targetId_))
            {
                const auto handle = imgui_->textureNativeHandle(*tex);
                if (handle)
                {
                    const auto texSize = tex->getSize();
                    const float texW = static_cast<float>(texSize.v[0]);
                    const float texH = static_cast<float>(texSize.v[1]);
                    const float availW = ImGui::GetContentRegionAvail().x;
                    const float maxW = std::min(availW - 8.0F, 320.0F);
                    const float scale = (texW > 0.0F)
                        ? std::min(1.0F, maxW / texW)
                        : 1.0F;
                    const float drawW = texW * scale;
                    const float drawH = texH * scale;

                    const ImVec2 cursor = ImGui::GetCursorScreenPos();
                    ImGui::Image(static_cast<ImTextureID>(handle),
                                 ImVec2(drawW, drawH));

                    // Draw cell outlines on top of the image so the
                    // user can preview exactly which cells will get
                    // slice rectangles. Uses ImGui's window draw
                    // list to overlay above the Image.
                    if (workingSet_.mode == SpriteMode::sliced &&
                        columns_ > 0 && rows_ > 0)
                    {
                        ImDrawList* dl = ImGui::GetWindowDrawList();
                        const ImU32 col = IM_COL32(255, 200, 60, 200);
                        const float cellW = drawW / static_cast<float>(columns_);
                        const float cellH = drawH / static_cast<float>(rows_);
                        for (int c = 0; c <= columns_; ++c)
                        {
                            const float x = cursor.x + c * cellW;
                            dl->AddLine(ImVec2(x, cursor.y),
                                        ImVec2(x, cursor.y + drawH),
                                        col, 1.0F);
                        }
                        for (int r = 0; r <= rows_; ++r)
                        {
                            const float y = cursor.y + r * cellH;
                            dl->AddLine(ImVec2(cursor.x, y),
                                        ImVec2(cursor.x + drawW, y),
                                        col, 1.0F);
                        }
                    }
                }
            }
        }

        // Compact slice list: count + first few names for sanity.
        ImGui::Separator();
        ImGui::Text("Slices: %zu", workingSet_.slices.size());
        if (!workingSet_.slices.empty())
        {
            const std::size_t showCount = std::min<std::size_t>(
                4, workingSet_.slices.size());
            for (std::size_t i = 0; i < showCount; ++i)
            {
                const auto& s = workingSet_.slices[i];
                ImGui::BulletText("%s  [%.0f, %.0f, %.0f, %.0f]",
                                  s.name.c_str(),
                                  s.rectPx.position.v[0],
                                  s.rectPx.position.v[1],
                                  s.rectPx.size.v[0],
                                  s.rectPx.size.v[1]);
            }
            if (workingSet_.slices.size() > showCount)
                ImGui::TextDisabled("...and %zu more",
                                    workingSet_.slices.size() - showCount);
        }

        ImGui::End();
    }
}
