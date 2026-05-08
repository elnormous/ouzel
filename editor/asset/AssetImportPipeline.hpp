#ifndef FLUXION_EDITOR_ASSETIMPORTPIPELINE_HPP
#define FLUXION_EDITOR_ASSETIMPORTPIPELINE_HPP

#include <cstddef>

namespace fluxion::assets { class Bundle; }

namespace fluxion::editor
{
    class AssetDatabase;
    struct AssetEntry;

    struct ImportSummary final
    {
        std::size_t imported = 0;
        std::size_t skipped = 0;     // entries with no loader (e.g. unknown type)
        std::size_t failed = 0;      // loader threw or returned false
    };

    // Push one entry through the engine's loader chain. Looks up the
    // matching loader by Asset::Type, calls Bundle::loadAsset with
    // the entry's stable Uuid so the resulting Cache resource is
    // findable both by name and by id. Returns true on success.
    //
    // Caller responsibility: the engine's storage::FileSystem must
    // know how to find the file at entry.relativePath — i.e. the
    // project root must be on its resourcePaths. EditorApp wires
    // that up at startup.
    bool importEntry(assets::Bundle& bundle, const AssetEntry& entry);

    // Convenience: walks every importable entry in the database and
    // funnels it through importEntry. Failures are logged but don't
    // abort the rest of the import — partial success is the common
    // case (one corrupt asset shouldn't blank out the project).
    ImportSummary importAll(assets::Bundle& bundle, const AssetDatabase& database);
}

#endif // FLUXION_EDITOR_ASSETIMPORTPIPELINE_HPP
