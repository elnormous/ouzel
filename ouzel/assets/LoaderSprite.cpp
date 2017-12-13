// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "LoaderSprite.hpp"
#include "Cache.hpp"
#include "scene/SpriteData.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderSprite::LoaderSprite():
            Loader({"json"})
        {
        }

        bool LoaderSprite::loadAsset(const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps)
        {
            scene::SpriteData spriteData;

            if (!spriteData.init(data, mipmaps))
            {
                return false;
            }

            cache->setSpriteData(filename, spriteData);

            return true;
        }
    } // namespace assets
} // namespace ouzel
