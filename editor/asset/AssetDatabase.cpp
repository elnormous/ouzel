#include "AssetDatabase.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include "../../engine/formats/Json.hpp"
#include "../../engine/storage/AtomicFile.hpp"
#include "../../engine/storage/Path.hpp"
#include "../../engine/utils/Log.hpp"

namespace fluxion::editor
{
    namespace
    {
        constexpr int kMetaSchemaVersion = 1;
        constexpr const char* kMetaExtension = ".meta";

        // ASCII-only lowercasing keeps extension matching predictable
        // even when the filename has Unicode in it (we only ever
        // compare extensions, never filenames).
        std::string toLower(std::string s)
        {
            for (auto& c : s)
                c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            return s;
        }

        // Extension -> Asset::Type mapping. The single-image PNG/JPG
        // case maps to image; .json is ambiguous (sprite vs cue) and
        // disambiguated by sniffing the file's first object keys.
        // Returns std::nullopt for files we don't know about — the
        // database still tracks them (so the browser can list raw
        // files) but the import pipeline skips them.
        std::optional<assets::Asset::Type> typeFromExtension(const std::string& ext)
        {
            using T = assets::Asset::Type;
            if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" ||
                ext == ".bmp" || ext == ".tga") return T::image;
            if (ext == ".wav") return T::sound;
            if (ext == ".ogg") return T::sound;
            if (ext == ".ttf") return T::font;
            if (ext == ".fnt") return T::font;
            if (ext == ".obj") return T::staticMesh;
            if (ext == ".dae") return T::skinnedMesh;
            if (ext == ".gltf") return T::skinnedMesh;
            if (ext == ".glb") return T::skinnedMesh;
            if (ext == ".mtl") return T::material;
            if (ext == ".json") return T::sprite;       // refined by sniffJsonType below
            return std::nullopt;
        }

        // Look at the parsed JSON to decide between sprite and cue.
        // TexturePacker JSONs always have a "frames" array and a
        // "meta" object; SpriteSheet JSONs (auto-grid) have
        // "filename" + "spritesX". Cue JSONs have a "source" object.
        // Anything else falls back to sprite (the more common case).
        assets::Asset::Type sniffJsonType(const std::filesystem::path& filePath)
        {
            using T = assets::Asset::Type;
            try
            {
                const auto text = storage::readTextFile(storage::Path{filePath.string()});
                const auto v = json::parse(text);
                if (!v.is<json::Object>()) return T::sprite;
                if (v.hasMember("source")) return T::cue;
                if (v.hasMember("frames") && v.hasMember("meta")) return T::sprite;
                if (v.hasMember("spritesX") && v.hasMember("filename")) return T::sprite;
                return T::sprite;
            }
            catch (...)
            {
                return T::sprite;
            }
        }

        // Build the JSON body for a fresh .meta sidecar. Encodes the
        // schema version, the v4 Uuid, the sniffed type and the
        // import options. Editor-only: not part of the engine's scene
        // serialisation surface.
        std::string buildMetaJson(const Uuid& id, assets::Asset::Type type,
                                  const assets::Asset::Options& options)
        {
            json::Value root;
            root = json::Object{};
            root["fluxion_meta_version"] = static_cast<std::int64_t>(kMetaSchemaVersion);
            root["id"] = id.toString();
            root["type"] = std::string{assets::typeName(type)};

            json::Value opts;
            opts = json::Object{};
            opts["mipmaps"] = options.mipmaps;
            root["options"] = opts;

            return json::encode(root, /*whitespace*/ true);
        }

        struct ParsedMeta final
        {
            Uuid id;
            std::optional<assets::Asset::Type> type;
            assets::Asset::Options options;
            SpriteSliceSet sliceSet;     // mode=single by default
        };

        // Read 4-element JSON array [x, y, w, h] into a Rect. Returns
        // a zero rect when the array is missing or wrong-shaped.
        math::Rect<float> parseRect(const json::Value& v)
        {
            math::Rect<float> r{};
            if (!v.is<json::Array>() || v.getSize() != 4) return r;
            r.position.v[0] = v[0].as<float>();
            r.position.v[1] = v[1].as<float>();
            r.size.v[0] = v[2].as<float>();
            r.size.v[1] = v[3].as<float>();
            return r;
        }

        json::Value rectToJson(const math::Rect<float>& r)
        {
            json::Value arr;
            arr = json::Array{};
            arr.pushBack(json::Value{static_cast<double>(r.position.v[0])});
            arr.pushBack(json::Value{static_cast<double>(r.position.v[1])});
            arr.pushBack(json::Value{static_cast<double>(r.size.v[0])});
            arr.pushBack(json::Value{static_cast<double>(r.size.v[1])});
            return arr;
        }

        std::optional<ParsedMeta> parseMetaJson(const std::filesystem::path& metaPath)
        {
            try
            {
                const auto text = storage::readTextFile(storage::Path{metaPath.string()});
                const auto root = json::parse(text);
                if (!root.is<json::Object>()) return std::nullopt;

                if (!root.hasMember("id")) return std::nullopt;
                const auto idStr = root["id"].as<std::string>();
                const auto parsed = Uuid::parse(idStr);
                if (!parsed) return std::nullopt;

                ParsedMeta out;
                out.id = *parsed;

                if (root.hasMember("type"))
                    out.type = assets::parseTypeName(root["type"].as<std::string>());

                if (root.hasMember("options") && root["options"].is<json::Object>())
                {
                    const auto& opts = root["options"];
                    if (opts.hasMember("mipmaps"))
                        out.options.mipmaps = opts["mipmaps"].as<bool>();
                }

                // Sprite-mode + slices (phase 12c). All optional —
                // older .meta sidecars from before 12c just skip them
                // and stay as mode=single, which preserves their
                // pre-12c behaviour.
                if (root.hasMember("sprite_mode") && root["sprite_mode"].is<std::string>())
                {
                    const auto modeStr = root["sprite_mode"].as<std::string>();
                    if (modeStr == "sliced") out.sliceSet.mode = SpriteMode::sliced;
                    else                     out.sliceSet.mode = SpriteMode::single;
                }
                if (root.hasMember("grid") && root["grid"].is<json::Object>())
                {
                    const auto& g = root["grid"];
                    if (g.hasMember("columns")) out.sliceSet.columns = g["columns"].as<int>();
                    if (g.hasMember("rows"))    out.sliceSet.rows    = g["rows"].as<int>();
                }
                if (root.hasMember("slices") && root["slices"].is<json::Array>())
                {
                    for (const auto& s : root["slices"])
                    {
                        if (!s.is<json::Object>()) continue;
                        SpriteSlice slice;
                        if (s.hasMember("id"))
                            if (auto sid = Uuid::parse(s["id"].as<std::string>()))
                                slice.id = *sid;
                        if (s.hasMember("name"))
                            slice.name = s["name"].as<std::string>();
                        if (s.hasMember("rect"))
                            slice.rectPx = parseRect(s["rect"]);
                        if (slice.id.isNull()) continue;  // bad entry; skip
                        out.sliceSet.slices.push_back(std::move(slice));
                    }
                }
                return out;
            }
            catch (...)
            {
                return std::nullopt;
            }
        }
    }

    bool AssetDatabase::setProjectRoot(const std::string& path)
    {
        std::error_code ec;
        const auto abs = std::filesystem::absolute(path, ec);
        if (ec) return false;
        if (!std::filesystem::is_directory(abs, ec)) return false;

        // Normalise: remove trailing separator, generic-form so the
        // database itself is platform-portable when serialised
        // (project settings file later).
        auto normalised = abs.lexically_normal().generic_string();
        while (!normalised.empty() && normalised.back() == '/')
            normalised.pop_back();

        projectRoot_ = std::move(normalised);
        entries_.clear();
        byId_.clear();
        byPath_.clear();
        return true;
    }

    AssetDatabase::ScanResult AssetDatabase::rescan()
    {
        ScanResult result{};
        if (projectRoot_.empty()) return result;

        entries_.clear();
        byId_.clear();
        byPath_.clear();
        sliceSets_.clear();

        const std::filesystem::path rootPath{projectRoot_};
        std::error_code walkEc;

        for (auto it = std::filesystem::recursive_directory_iterator{rootPath, walkEc};
             it != std::filesystem::recursive_directory_iterator{}; it.increment(walkEc))
        {
            if (walkEc)
            {
                log(Log::Level::warning) << "AssetDatabase: walk error: " << walkEc.message();
                walkEc.clear();
                continue;
            }
            if (!it->is_regular_file()) continue;

            const auto& filePath = it->path();
            const auto ext = toLower(filePath.extension().string());
            if (ext == kMetaExtension) continue; // sidecars themselves don't get entries

            const auto relPath = std::filesystem::relative(filePath, rootPath, walkEc);
            if (walkEc)
            {
                walkEc.clear();
                continue;
            }
            const auto relGeneric = relPath.generic_string();

            ++result.scanned;

            // Decide the asset type. Extension-based; .json gets a
            // peek to disambiguate sprite vs cue.
            auto sniffed = typeFromExtension(ext);
            const bool importable = sniffed.has_value();
            if (sniffed && *sniffed == assets::Asset::Type::sprite && ext == ".json")
                sniffed = sniffJsonType(filePath);

            const assets::Asset::Type effectiveType =
                sniffed.value_or(assets::Asset::Type::image);

            // Read or create the .meta sidecar. The sidecar is the
            // canonical id store; if it's missing, mint a fresh
            // Uuid and atomically write a fresh .meta. If it exists
            // but is unparseable, log and skip — destructive
            // overwrites would clobber whatever the user/another
            // tool stashed there.
            const std::filesystem::path metaPath =
                filePath.string() + kMetaExtension;

            assets::Asset::Options options;
            assets::Asset::Type type = effectiveType;
            Uuid id;
            SpriteSliceSet sliceSet;       // mode=single by default

            if (std::filesystem::exists(metaPath, walkEc))
            {
                walkEc.clear();
                if (auto parsed = parseMetaJson(metaPath))
                {
                    id = parsed->id;
                    if (parsed->type) type = *parsed->type;
                    options = parsed->options;
                    sliceSet = std::move(parsed->sliceSet);
                    ++result.metaLoaded;
                }
                else
                {
                    log(Log::Level::warning)
                        << "AssetDatabase: invalid .meta for "
                        << relGeneric << ", skipping";
                    ++result.metaInvalid;
                    continue;
                }
            }
            else
            {
                walkEc.clear();
                id = Uuid::generate();
                try
                {
                    storage::atomicWriteFile(storage::Path{metaPath.string()},
                                             buildMetaJson(id, type, options));
                    ++result.metaCreated;
                }
                catch (const std::exception& e)
                {
                    log(Log::Level::warning)
                        << "AssetDatabase: failed to write .meta for "
                        << relGeneric << ": " << e.what();
                    continue;
                }
            }

            // Reject duplicate ids — first writer wins, log the
            // second so the user can fix the sidecar manually.
            if (byId_.find(id) != byId_.end())
            {
                log(Log::Level::warning)
                    << "AssetDatabase: duplicate id "
                    << id.toString() << " for "
                    << relGeneric << ", ignoring";
                continue;
            }

            AssetEntry entry;
            entry.id = id;
            entry.type = type;
            entry.relativePath = relGeneric;
            entry.name = filePath.stem().string();
            entry.extension = ext;
            entry.importable = importable;
            entry.options = options;

            const auto index = entries_.size();
            entries_.push_back(std::move(entry));
            byId_.emplace(id, index);
            byPath_.emplace(relGeneric, index);

            // Emit a sub-entry for every saved slice on this parent.
            // Each slice keeps the parent's options + relativePath
            // suffixed with #<sliceName> so the AssetBrowser can
            // tell them apart visually. The slice's own Uuid is what
            // the SpriteRenderer's AssetReference will resolve to —
            // each slice ends up in cache.getSpriteData() through
            // AssetImportPipeline.
            if (sliceSet.mode == SpriteMode::sliced && !sliceSet.slices.empty())
            {
                for (const auto& slice : sliceSet.slices)
                {
                    if (byId_.find(slice.id) != byId_.end()) continue;
                    AssetEntry sub;
                    sub.id = slice.id;
                    sub.type = assets::Asset::Type::sprite;
                    sub.relativePath = relGeneric + "#" + slice.name;
                    sub.name = slice.name;
                    sub.extension = ext;
                    sub.importable = true;
                    sub.options = options;
                    sub.parentId = id;
                    sub.sliceRectPx = slice.rectPx;

                    const auto subIndex = entries_.size();
                    entries_.push_back(std::move(sub));
                    byId_.emplace(slice.id, subIndex);
                    byPath_.emplace(entries_.back().relativePath, subIndex);
                }
                sliceSets_.emplace(id, std::move(sliceSet));
            }
        }

        return result;
    }

    const AssetEntry* AssetDatabase::findById(const Uuid& id) const
    {
        const auto i = byId_.find(id);
        return (i == byId_.end()) ? nullptr : &entries_[i->second];
    }

    const AssetEntry* AssetDatabase::findByRelativePath(const std::string& relativePath) const
    {
        const auto i = byPath_.find(relativePath);
        return (i == byPath_.end()) ? nullptr : &entries_[i->second];
    }

    std::string AssetDatabase::absolutePath(const AssetEntry& entry) const
    {
        if (projectRoot_.empty()) return entry.relativePath;
        // Sub-slice entries carry "<parent>#<sliceName>" as the
        // relativePath; the actual file on disk is the parent only.
        // Strip the "#..." suffix here so callers (preview readers,
        // SpriteEditor) get a path that opens.
        const auto hashPos = entry.relativePath.find('#');
        const auto onDisk = (hashPos == std::string::npos)
            ? entry.relativePath
            : entry.relativePath.substr(0, hashPos);
        return projectRoot_ + "/" + onDisk;
    }

    SpriteSliceSet AssetDatabase::getSliceSet(const Uuid& parentId) const
    {
        if (const auto i = sliceSets_.find(parentId); i != sliceSets_.end())
            return i->second;
        return SpriteSliceSet{};
    }

    bool AssetDatabase::renameAsset(const Uuid& id, const std::string& newName)
    {
        const auto* entry = findById(id);
        if (!entry || !entry->parentId.isNull()) return false;
        if (newName.empty()) return false;
        if (projectRoot_.empty()) return false;

        // Compose the new path. If the user didn't type an
        // extension, preserve the existing one — that's the typical
        // case (rename "player" → "player.png" without forcing them
        // to retype the suffix).
        const std::filesystem::path oldPath{absolutePath(*entry)};
        const auto oldDir = oldPath.parent_path();
        std::filesystem::path newFilename{newName};
        if (newFilename.extension().empty() && !oldPath.extension().empty())
            newFilename += oldPath.extension();
        const auto newPath = oldDir / newFilename;
        const auto oldMeta = std::filesystem::path{oldPath.string() + kMetaExtension};
        const auto newMeta = std::filesystem::path{newPath.string() + kMetaExtension};

        std::error_code ec;
        if (std::filesystem::exists(newPath, ec))
        {
            log(Log::Level::warning)
                << "AssetDatabase: rename target already exists: "
                << newPath.string();
            return false;
        }
        std::filesystem::rename(oldPath, newPath, ec);
        if (ec)
        {
            log(Log::Level::warning)
                << "AssetDatabase: rename failed: " << ec.message();
            return false;
        }
        // .meta may not exist yet for unmetafied files — ignore that
        // case rather than failing the whole rename.
        if (std::filesystem::exists(oldMeta, ec))
        {
            std::filesystem::rename(oldMeta, newMeta, ec);
            if (ec)
                log(Log::Level::warning)
                    << "AssetDatabase: .meta rename failed: " << ec.message();
        }

        rescan();
        return true;
    }

    bool AssetDatabase::deleteAsset(const Uuid& id)
    {
        const auto* entry = findById(id);
        if (!entry || !entry->parentId.isNull()) return false;
        if (projectRoot_.empty()) return false;

        const std::filesystem::path filePath{absolutePath(*entry)};
        const auto metaPath = std::filesystem::path{filePath.string() + kMetaExtension};

        std::error_code ec;
        std::filesystem::remove(filePath, ec);
        if (ec)
        {
            log(Log::Level::warning)
                << "AssetDatabase: delete failed: " << ec.message();
            return false;
        }
        // Best-effort .meta cleanup; missing meta isn't a hard fail.
        std::filesystem::remove(metaPath, ec);

        rescan();
        return true;
    }

    bool AssetDatabase::saveSliceSet(const Uuid& parentId,
                                     const SpriteSliceSet& set)
    {
        // Find the parent entry to locate its .meta on disk.
        const auto* parent = findById(parentId);
        if (!parent || !parent->parentId.isNull()) return false;

        // Strip the "#..." suffix (parents never carry it, but the
        // helper in absolutePath handles it harmlessly).
        const auto metaPath = std::filesystem::path{absolutePath(*parent)
                                                    + std::string{kMetaExtension}};

        // Parse the existing meta, preserving fields we don't touch.
        json::Value root;
        try
        {
            const auto text = storage::readTextFile(storage::Path{metaPath.string()});
            root = json::parse(text);
            if (!root.is<json::Object>()) root = json::Object{};
        }
        catch (...)
        {
            root = json::Object{};
        }

        // Mode + grid.
        root["sprite_mode"] = std::string{
            set.mode == SpriteMode::sliced ? "sliced" : "single"};
        json::Value grid;
        grid = json::Object{};
        grid["columns"] = static_cast<std::int64_t>(set.columns);
        grid["rows"]    = static_cast<std::int64_t>(set.rows);
        root["grid"] = grid;

        // Slices array. Each slice keeps its existing Uuid; the
        // editor matches them by index when re-applying a grid.
        json::Value slices;
        slices = json::Array{};
        for (const auto& s : set.slices)
        {
            json::Value entry;
            entry = json::Object{};
            entry["id"] = s.id.toString();
            entry["name"] = s.name;
            entry["rect"] = rectToJson(s.rectPx);
            slices.pushBack(entry);
        }
        root["slices"] = slices;

        // Atomic write — same I/O policy as fresh-meta creation.
        try
        {
            storage::atomicWriteFile(storage::Path{metaPath.string()},
                                     json::encode(root, /*whitespace*/ true));
        }
        catch (const std::exception& e)
        {
            log(Log::Level::warning)
                << "AssetDatabase: failed to save slice set for "
                << parent->relativePath << ": " << e.what();
            return false;
        }

        // Rescan to refresh entries_ and sliceSets_ in one shot.
        // Cheaper paths (in-place patch) are possible but the rescan
        // also picks up renames / new files the user may have made
        // in the meantime, so this is the safer default.
        rescan();
        return true;
    }
}
