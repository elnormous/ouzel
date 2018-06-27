// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
            font->init(data, mipmaps);

            cache->setFont(filename, font);

            return true;
        }
    } // namespace assets
} // namespace ouzel
