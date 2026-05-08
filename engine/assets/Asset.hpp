#ifndef FLUXION_ASSETS_ASSET_HPP
#define FLUXION_ASSETS_ASSET_HPP

#include <string>
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
}

#endif // FLUXION_ASSETS_ASSET_HPP
