// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "LoaderTTF.hpp"
#include "Cache.hpp"
#include "gui/TTFont.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderTTF::LoaderTTF():
            Loader(TYPE, {"ttf"})
        {
        }

        bool LoaderTTF::loadAsset(const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps)
        {
            std::shared_ptr<TTFont> font = std::make_shared<TTFont>();
            if (!font->init(data, mipmaps))
                return false;

            cache->setFont(filename, font);

            return true;
        }
    } // namespace assets
} // namespace ouzel
