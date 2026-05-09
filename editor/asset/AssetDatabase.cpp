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
        };

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

            if (std::filesystem::exists(metaPath, walkEc))
            {
                walkEc.clear();
                if (auto parsed = parseMetaJson(metaPath))
                {
                    id = parsed->id;
                    if (parsed->type) type = *parsed->type;
                    options = parsed->options;
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
        return projectRoot_ + "/" + entry.relativePath;
    }
}
