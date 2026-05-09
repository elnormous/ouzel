#ifndef FLUXION_ASSETS_ASSET_HPP
#define FLUXION_ASSETS_ASSET_HPP

#include <optional>
#include <string>
#include <string_view>
#include "../utils/Uuid.hpp"

namespace fluxion::assets
{
    class Asset final
    {
    public:
        enum class Type
        {
            bank,
            font,
            staticMesh,
            skinnedMesh,
            image,
            material,
            particleSystem,
            sprite,
            sound,
            cue
        };

        struct Options final
        {
            bool mipmaps = true;
        };

        Asset(Type initType,
              const std::string& initName,
              const std::string& initFilename,
              const Options& initOptions):
            type{initType},
            name{initName},
            filename{initFilename},
            options{initOptions}
        {
        }

        Type type;
        std::string name;
        std::string filename;
        Options options;
    };

    // Stable cross-scene identifier for a loaded asset. Scene files
    // store this; the AssetDatabase resolves it back to a Cache
    // resource at load time. A null id encodes "no asset assigned".
    struct AssetReference final
    {
        Uuid id;
        Asset::Type type{Asset::Type::image};

        [[nodiscard]] bool isNull() const noexcept { return id.isNull(); }

        [[nodiscard]] bool operator==(const AssetReference& other) const noexcept
        {
            return id == other.id && type == other.type;
        }
        [[nodiscard]] bool operator!=(const AssetReference& other) const noexcept
        {
            return !(*this == other);
        }
    };

    // Stable string form of Asset::Type. Used by .meta sidecars and
    // scene-file AssetReferences so reordering or inserting enum
    // values in the source code doesn't silently change the meaning
    // of saved data. The names match the enumerator spelling.
    [[nodiscard]] inline const char* typeName(Asset::Type t) noexcept
    {
        switch (t)
        {
            case Asset::Type::bank: return "bank";
            case Asset::Type::font: return "font";
            case Asset::Type::staticMesh: return "staticMesh";
            case Asset::Type::skinnedMesh: return "skinnedMesh";
            case Asset::Type::image: return "image";
            case Asset::Type::material: return "material";
            case Asset::Type::particleSystem: return "particleSystem";
            case Asset::Type::sprite: return "sprite";
            case Asset::Type::sound: return "sound";
            case Asset::Type::cue: return "cue";
        }
        return "image";
    }

    [[nodiscard]] inline std::optional<Asset::Type>
    parseTypeName(std::string_view s) noexcept
    {
        using T = Asset::Type;
        if (s == "bank") return T::bank;
        if (s == "font") return T::font;
        if (s == "staticMesh") return T::staticMesh;
        if (s == "skinnedMesh") return T::skinnedMesh;
        if (s == "image") return T::image;
        if (s == "material") return T::material;
        if (s == "particleSystem") return T::particleSystem;
        if (s == "sprite") return T::sprite;
        if (s == "sound") return T::sound;
        if (s == "cue") return T::cue;
        return std::nullopt;
    }
}

#endif // FLUXION_ASSETS_ASSET_HPP
