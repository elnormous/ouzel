#ifndef FLUXION_EDITOR_ASSETDATABASE_HPP
#define FLUXION_EDITOR_ASSETDATABASE_HPP

#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "../../engine/assets/Asset.hpp"
#include "../../engine/math/Rect.hpp"
#include "../../engine/utils/Uuid.hpp"

namespace fluxion::editor
{
    // One discovered asset file inside the project root. Path is the
    // generic-form (forward-slash) path relative to the root, so it
    // round-trips through scene files and .meta sidecars unchanged
    // across platforms.
    //
    // type is the engine Asset::Type sniffed from the file's
    // extension (or, for .json, from a tiny content peek). Files
    // whose type cannot be determined keep type=image with
    // importable=false — they show up in the database (so the
    // browser can list them) but the import pipeline won't try to
    // push them through Bundle::loadAsset.
    struct AssetEntry final
    {
        Uuid id;
        assets::Asset::Type type{assets::Asset::Type::image};
        std::string relativePath;     // e.g. "subdir/player.png"
        std::string name;             // e.g. "player" (stem, no extension)
        std::string extension;        // lowercased, with leading dot ("" if none)
        bool importable = false;
        assets::Asset::Options options;

        // Sub-sprite plumbing (phase 12c). When the user opens an
        // image in the Sprite Editor and slices it, each cell becomes
        // a derivative AssetEntry whose `parentId` points back at the
        // image's entry; `sliceRectPx` is the cell rectangle in
        // parent-texture pixel coordinates. Top-level entries leave
        // parentId null and sliceRectPx zeroed. The slice's
        // relativePath is "<parent>#<sliceName>" — the "#" is purely
        // a UI convention so users can tell parents and slices apart
        // in the AssetBrowser.
        Uuid parentId;
        math::Rect<float> sliceRectPx;
    };

    // Metadata for the Sprite Editor — describes how a parent texture
    // is sliced into named cells. Lives in the parent's .meta sidecar.
    // 12c-1 ships only the auto-grid path (`SpriteMode::sliced`); the
    // interactive freeform editor (Multiple mode) is a future
    // extension and will reuse this same shape with grid=0.
    struct SpriteSlice final
    {
        Uuid id;
        std::string name;
        math::Rect<float> rectPx;     // (x, y, w, h) in parent texture
                                       // pixel coordinates
    };

    enum class SpriteMode
    {
        single,    // texture is one sprite (no sub-slices)
        sliced,    // auto-grid; slices generated from columns x rows
    };

    struct SpriteSliceSet final
    {
        SpriteMode mode{SpriteMode::single};
        int columns{1};
        int rows{1};
        std::vector<SpriteSlice> slices;
    };

    // Editor-owned single source of truth for project assets.
    //
    // Lifecycle:
    //   1. setProjectRoot(absolutePath) tells the database where the
    //      project lives.
    //   2. rescan() walks the tree recursively, reads/writes the
    //      `<file>.meta` JSON sidecar per asset, and populates the
    //      Uuid <-> path tables. The .meta is the source of truth
    //      for the GUID; first scan generates one for every
    //      unmetafied asset and atomic-writes the sidecar so the
    //      identity survives the next run.
    //   3. The browser/inspector look up entries by id (when scenes
    //      reference an asset) or by relative path (when the user
    //      drops a file in).
    //
    // Cross-platform: uses <filesystem> for the directory walk;
    // fluxion::storage::atomicWriteFile + readTextFile for the
    // sidecar I/O. No new platform-specific code.
    class AssetDatabase final
    {
    public:
        AssetDatabase() = default;
        AssetDatabase(const AssetDatabase&) = delete;
        AssetDatabase& operator=(const AssetDatabase&) = delete;

        // Switch project root. Does NOT rescan automatically — call
        // rescan() afterwards. The path may be absolute or relative
        // to the editor's current working directory; both are
        // resolved and stored as a normalised absolute path.
        // Returns true on success, false if the path doesn't exist
        // or isn't a directory.
        bool setProjectRoot(const std::string& path);

        [[nodiscard]] const std::string& getProjectRoot() const noexcept { return projectRoot_; }
        [[nodiscard]] bool isOpen() const noexcept { return !projectRoot_.empty(); }

        struct ScanResult final
        {
            std::size_t scanned = 0;       // total non-.meta files seen
            std::size_t metaCreated = 0;   // .meta sidecars freshly written
            std::size_t metaLoaded = 0;    // .meta sidecars successfully parsed
            std::size_t metaInvalid = 0;   // .meta sidecars with bad/missing id
        };

        // Walk the project root and reconcile entries with .meta
        // sidecars. Idempotent: a second call after no filesystem
        // change returns scanned=N, metaCreated=0, metaLoaded=N.
        ScanResult rescan();

        [[nodiscard]] const AssetEntry* findById(const Uuid& id) const;
        [[nodiscard]] const AssetEntry* findByRelativePath(const std::string& relativePath) const;
        [[nodiscard]] const std::vector<AssetEntry>& entries() const noexcept { return entries_; }

        // Absolute path on disk to the file backing this entry.
        // Useful for opening the file directly (e.g. preview).
        [[nodiscard]] std::string absolutePath(const AssetEntry& entry) const;

        // Slice-set accessor. Returns the parent's last-saved set, or
        // an empty default (mode=single) when the parent has no
        // sprite_mode entry in its .meta yet. Look-up is by parent's
        // image-asset id.
        [[nodiscard]] SpriteSliceSet getSliceSet(const Uuid& parentId) const;

        // Save a slice-set on the parent's .meta and rebuild the
        // sub-entries in entries_ accordingly. Each slice keeps its
        // existing Uuid across saves where possible (matched by
        // index) so already-saved scenes don't lose their refs when
        // the user re-applies a different grid. Returns true on
        // success, false on filesystem / json errors.
        bool saveSliceSet(const Uuid& parentId, const SpriteSliceSet& set);

        // Rename a top-level asset's file on disk (and its .meta
        // sidecar) to `newName`. Sub-slice entries (parentId set)
        // can't be renamed this way — their names live inside the
        // parent's .meta and are edited via the SpriteEditor.
        // newName is the new filename only (extension preserved
        // when not given). Returns true on success.
        bool renameAsset(const Uuid& id, const std::string& newName);

        // Delete a top-level asset's file + .meta from disk. Sub-
        // slices vanish via the natural rescan (their parent is
        // gone). The Uuid is no longer reachable through findById
        // after this call. Returns true on success.
        bool deleteAsset(const Uuid& id);

    private:
        std::string projectRoot_;            // normalised absolute, no trailing slash
        std::vector<AssetEntry> entries_;
        std::unordered_map<Uuid, std::size_t> byId_;       // id -> index into entries_
        std::map<std::string, std::size_t> byPath_;        // relativePath -> index

        // Per-parent slice metadata (mode + grid + slice list). Read
        // from .meta during rescan, mutated by saveSliceSet, written
        // back atomically. Parents without a sprite_mode entry don't
        // appear here — getSliceSet returns a default for them.
        std::unordered_map<Uuid, SpriteSliceSet> sliceSets_;
    };
}

#endif // FLUXION_EDITOR_ASSETDATABASE_HPP
