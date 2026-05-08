#include "AssetImportPipeline.hpp"

#include <exception>
#include "AssetDatabase.hpp"
#include "../../engine/assets/Bundle.hpp"
#include "../../engine/utils/Log.hpp"

namespace fluxion::editor
{
    bool importEntry(assets::Bundle& bundle, const AssetEntry& entry)
    {
        if (!entry.importable) return false;
        try
        {
            // Pass the .meta-stored Uuid so the Cache resource
            // becomes findable via cache.getX(entry.id) too. The
            // filename argument is what FileSystem::readFile sees:
            // a project-root-relative generic path (forward slashes)
            // the engine resolves via its resource search list.
            bundle.loadAsset(entry.type, entry.name, entry.relativePath,
                             entry.options, entry.id);
            return true;
        }
        catch (const std::exception& e)
        {
            log(Log::Level::warning)
                << "AssetImportPipeline: failed to import "
                << entry.relativePath << ": " << e.what();
            return false;
        }
    }

    ImportSummary importAll(assets::Bundle& bundle, const AssetDatabase& database)
    {
        ImportSummary summary{};
        for (const auto& entry : database.entries())
        {
            if (!entry.importable) { ++summary.skipped; continue; }
            if (importEntry(bundle, entry)) ++summary.imported;
            else                            ++summary.failed;
        }
        return summary;
    }
}
