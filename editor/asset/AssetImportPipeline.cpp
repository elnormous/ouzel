#include "AssetImportPipeline.hpp"

#include <exception>
#include "AssetDatabase.hpp"
#include "../../engine/assets/Bundle.hpp"
#include "../../engine/assets/Cache.hpp"
#include "../../engine/core/Engine.hpp"
#include "../../engine/scene/SpriteRenderer.hpp"
#include "../../engine/utils/Log.hpp"

namespace fluxion::editor
{
    namespace
    {
        // For a sub-slice entry (parentId set, type=sprite), build a
        // SpriteData that points at the parent's texture and carries
        // a single frame matching the slice's pixel rectangle. The
        // resulting SpriteData lives in the bundle keyed by both the
        // slice's display name and its Uuid, so cache.getSpriteData()
        // resolves it both ways.
        bool importSlice(assets::Bundle& bundle, const AssetEntry& entry)
        {
            if (entry.parentId.isNull()) return false;
            const auto parentTex = bundle.getTexture(entry.parentId);
            if (!parentTex) return false;     // parent not yet imported

            const auto texSize = parentTex->getSize();
            const math::Size<float, 2> textureSize{
                static_cast<float>(texSize.v[0]),
                static_cast<float>(texSize.v[1])
            };
            const math::Size<float, 2> sourceSize{
                entry.sliceRectPx.size.v[0],
                entry.sliceRectPx.size.v[1]
            };

            scene::SpriteData data;
            data.texture = parentTex;
            // Default blend / shader come from cache lookups in the
            // SpriteRenderer's init path itself (engine fallback).
            // Here we just need texture + animations.

            scene::SpriteData::Animation anim;
            anim.frames.emplace_back(
                entry.name,
                textureSize,
                entry.sliceRectPx,
                /*rotated*/ false,
                sourceSize,
                math::Vector<float, 2>{0.0F, 0.0F},
                math::Vector<float, 2>{0.5F, 0.5F});
            data.animations[""] = std::move(anim);

            bundle.setSpriteData(entry.name, data, entry.id);
            return true;
        }
    }

    bool importEntry(assets::Bundle& bundle, const AssetEntry& entry)
    {
        if (!entry.importable) return false;

        // Sub-slice path: don't go through the engine's loader chain
        // (no on-disk file to read for slices). Build the SpriteData
        // directly from the parent texture + slice rect.
        if (!entry.parentId.isNull())
        {
            try
            {
                if (importSlice(bundle, entry)) return true;
                log(Log::Level::warning)
                    << "AssetImportPipeline: parent texture for slice "
                    << entry.relativePath << " not loaded yet";
                return false;
            }
            catch (const std::exception& e)
            {
                log(Log::Level::warning)
                    << "AssetImportPipeline: failed to register slice "
                    << entry.relativePath << ": " << e.what();
                return false;
            }
        }

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
        // Two passes: parents first so their textures are in the
        // cache before slice entries try to look them up. The order
        // in entries_ already places parents first by construction
        // (we append slices after their parent during rescan), but
        // an explicit two-pass keeps the dependency obvious if entry
        // ordering ever changes.
        for (const auto& entry : database.entries())
        {
            if (!entry.parentId.isNull()) continue;
            if (!entry.importable) { ++summary.skipped; continue; }
            if (importEntry(bundle, entry)) ++summary.imported;
            else                            ++summary.failed;
        }
        for (const auto& entry : database.entries())
        {
            if (entry.parentId.isNull()) continue;
            if (!entry.importable) { ++summary.skipped; continue; }
            if (importEntry(bundle, entry)) ++summary.imported;
            else                            ++summary.failed;
        }
        return summary;
    }

    ImportSummary importSlicesFor(assets::Bundle& bundle,
                                   const AssetDatabase& database,
                                   const Uuid& parentId)
    {
        ImportSummary summary{};
        if (parentId.isNull()) return summary;
        for (const auto& entry : database.entries())
        {
            if (entry.parentId != parentId) continue;
            if (!entry.importable) { ++summary.skipped; continue; }
            if (importEntry(bundle, entry)) ++summary.imported;
            else                            ++summary.failed;
        }
        return summary;
    }
}
