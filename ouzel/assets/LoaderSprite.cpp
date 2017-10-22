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

        bool LoaderSprite::loadAsset(const std::string& filename)
        {
            scene::SpriteData spriteData;

            if (!spriteData.load(filename))
            {
                return false;
            }

            cache->setSpriteData(filename, spriteData);

            return true;
        }
    } // namespace assets
} // namespace ouzel
