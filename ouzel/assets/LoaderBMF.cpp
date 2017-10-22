// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "LoaderBMF.hpp"
#include "Cache.hpp"
#include "gui/BMFont.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderBMF::LoaderBMF():
            Loader({"fnt"})
        {
        }

        bool LoaderBMF::loadAsset(const std::string& filename, const std::vector<uint8_t>& data)
        {
            std::shared_ptr<BMFont> font = std::make_shared<BMFont>();
            if (!font->init(data))
            {
                return false;
            }

            cache->setFont(filename, font);

            return true;
        }
    } // namespace assets
} // namespace ouzel
