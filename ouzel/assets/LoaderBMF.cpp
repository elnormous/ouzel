// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "LoaderBMF.hpp"
#include "Cache.hpp"
#include "gui/BMFont.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderBMF::LoaderBMF():
            Loader(TYPE, {"fnt"})
        {
        }

        bool LoaderBMF::loadAsset(const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps)
        {
            std::shared_ptr<BMFont> font = std::make_shared<BMFont>();
            font->init(data, mipmaps);

            cache->setFont(filename, font);

            return true;
        }
    } // namespace assets
} // namespace ouzel
