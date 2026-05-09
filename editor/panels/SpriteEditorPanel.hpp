#ifndef FLUXION_EDITOR_SPRITEEDITORPANEL_HPP
#define FLUXION_EDITOR_SPRITEEDITORPANEL_HPP

#include "../Panel.hpp"
#include "../asset/AssetDatabase.hpp"

namespace fluxion::assets { class Bundle; class Cache; }
namespace fluxion::gui::imgui { class Context; }

namespace fluxion::editor
{
    class AssetDatabase;

    // Edits a single image asset's sub-slice layout. 12c-1 ships only
    // the auto-grid path (`SpriteMode::sliced`): the user picks
    // columns x rows, the panel computes uniform cell rectangles in
    // parent-texture pixel coords, and Save writes them to the
    // .meta sidecar via AssetDatabase::saveSliceSet plus re-registers
    // the resulting SpriteData in the project's Bundle.
    //
    // The panel is non-modal: it lives among the other dockable
    // panels and only renders content when a target is set. The
    // AssetBrowser context menu calls setTarget(parentId) to open
    // the editor on a particular image.
    class SpriteEditorPanel final : public Panel
    {
    public:
        SpriteEditorPanel(AssetDatabase* database,
                          assets::Bundle* projectBundle,
                          assets::Cache* cache,
                          gui::imgui::Context* imgui) noexcept;

        void draw() override;

        // Open the editor on this asset id. Must be a top-level
        // image entry (parentId null, type=image) — anything else
        // is silently no-op'd. Calling with the null Uuid clears
        // the panel back to its empty state.
        void setTarget(const Uuid& parentId);

    private:
        // Recompute workingSet_.slices from columns_ x rows_ and the
        // parent texture's pixel size. Existing slice Uuids are
        // preserved by index where the new grid still has a cell at
        // that index, so already-saved scenes keep their references
        // when the user re-applies a different grid.
        void applyGrid();

        // Persist the working slice-set + re-register sub-slices in
        // the cache. Returns true on success.
        bool saveAndReimport();

        AssetDatabase* database_;
        assets::Bundle* projectBundle_;
        assets::Cache* cache_;
        gui::imgui::Context* imgui_;

        Uuid targetId_;
        SpriteSliceSet workingSet_;
        int columns_ = 1;
        int rows_ = 1;
    };
}

#endif // FLUXION_EDITOR_SPRITEEDITORPANEL_HPP
