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
            Cue
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
}

#endif // OUZEL_ASSET_HPP
