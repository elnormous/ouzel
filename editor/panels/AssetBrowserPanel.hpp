#ifndef FLUXION_EDITOR_ASSETBROWSERPANEL_HPP
#define FLUXION_EDITOR_ASSETBROWSERPANEL_HPP

#include <set>
#include <string>
#include "../Panel.hpp"

namespace fluxion::assets { class Cache; }
namespace fluxion::gui::imgui { class Context; }

namespace fluxion::editor
{
    class AssetDatabase;

    // Two-pane folder browser for the project's AssetDatabase.
    //
    // Left pane: collapsible folder tree built from the relative
    // paths exposed by AssetDatabase.entries(). Right pane: a
    // wrapping grid of asset tiles for the currently-selected
    // folder. Each tile is the drag-source for a "FLUXION_ASSET_REF"
    // payload (an AssetReference value) — Inspector receptors render
    // the corresponding drop target.
    //
    // Image previews go through the engine's ImGui context texture-
    // handle accessor (D3D11 / OpenGL / Metal all implement it the
    // same way) so the panel stays renderer-agnostic. Non-image
    // assets show a coloured tile + a single-letter type glyph as a
    // stand-in.
    class AssetBrowserPanel final : public Panel
    {
    public:
        AssetBrowserPanel(AssetDatabase* database,
                          assets::Cache* cache,
                          gui::imgui::Context* imgui) noexcept;

        void draw() override;

    private:
        // Build (or rebuild) folderSet_ from the database. Cheap;
        // called once per draw() so renames pick up automatically.
        void collectFolders();

        // Recursive folder-tree drawer. `prefix` is the relative path
        // accumulated so far ("" for the root); only direct children
        // of `prefix` get their own nodes — deeper folders recurse.
        void drawFolderNode(const std::string& prefix);

        AssetDatabase* database_;     // not owned
        assets::Cache* cache_;        // not owned, may be nullptr
        gui::imgui::Context* imgui_;  // not owned, may be nullptr

        // Currently-selected folder, relative to project root.
        // Empty string == project root itself.
        std::string selectedFolder_;

        // Cache of every folder path discovered during the last
        // collectFolders() pass. Sorted (std::set) so the tree
        // renders alphabetically without us having to sort manually.
        std::set<std::string> folderSet_;
    };
}

#endif // FLUXION_EDITOR_ASSETBROWSERPANEL_HPP
