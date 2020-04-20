// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

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
            Font,
            Mesh,
            Texture,
            Material,
            ParticleSystem,
            Sprite,
            Sound,
            Cue,
            Shader
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
        const Type type;
        const bool mipmaps = false;
    };

    inline Asset::Type stringToAssetType(const std::string& s)
    {
        if (s == "font")
            return Asset::Type::Font;
        else if (s == "mesh")
            return Asset::Type::Mesh;
        else if (s == "texture")
            return Asset::Type::Texture;
        else if (s == "material")
            return Asset::Type::Material;
        else if (s == "particle_system")
            return Asset::Type::ParticleSystem;
        else if (s == "sprite")
            return Asset::Type::Sprite;
        else if (s == "sound")
            return Asset::Type::Sound;
        else if (s == "cue")
            return Asset::Type::Cue;
        else if (s == "shader")
            return Asset::Type::Shader;
        else
            throw std::runtime_error("Invalid asset type");
    }
}

#endif // OUZEL_ASSET_HPP
