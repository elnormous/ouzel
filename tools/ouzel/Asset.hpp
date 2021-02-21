// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSET_HPP
#define OUZEL_ASSET_HPP

#include "storage/Path.hpp"

namespace ouzel
{
    class Asset final
    {
    public:
        enum class Type
        {
            empty,
            font,
            mesh,
            texture,
            material,
            particleSystem,
            sprite,
            sound,
            cue,
            shader
        };

        Asset(const storage::Path& initPath,
              const std::string& initName,
              Type initType,
              bool initMipmaps):
            path(initPath),
            name(initName),
            type(initType),
            mipmaps(initMipmaps) {}

        const storage::Path path;
        const std::string name;
        const Type type = Type::empty;
        const bool mipmaps = false;
    };

    inline Asset::Type stringToAssetType(const std::string& s)
    {
        if (s == "empty")
            return Asset::Type::empty;
        if (s == "font")
            return Asset::Type::font;
        else if (s == "mesh")
            return Asset::Type::mesh;
        else if (s == "texture")
            return Asset::Type::texture;
        else if (s == "material")
            return Asset::Type::material;
        else if (s == "particle_system")
            return Asset::Type::particleSystem;
        else if (s == "sprite")
            return Asset::Type::sprite;
        else if (s == "sound")
            return Asset::Type::sound;
        else if (s == "cue")
            return Asset::Type::cue;
        else if (s == "shader")
            return Asset::Type::shader;
        else
            throw std::runtime_error("Invalid asset type");
    }
}

#endif // OUZEL_ASSET_HPP
